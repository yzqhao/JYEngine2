local Math = require "Math"
local Core = require("MeteorCore")
local venusjson = require "venusjson"
local Engine = require "Engine"
local defined = require "window.editor.system.defined"
local BundleManager = require "window.editor.system.BundleManager"
local bitop = require "bit"
local filelfs = require "window.editor.widget.assets.fileutility"
require("framework.engine.SceneManager")
-- command
local DeserializeSceneCmd = require "window.editor.command.commands.scene.deserialize_scene_command"
local CommandManager = require "window.editor.command.command_manager"

local EditorSystem = {}

local ParticleDefaultName = "particlesystem"
local CameraDefaultName = "cameranode"
local EditorCameraName = "editorcamera"
local SpineDefaultName = "spinenode"
local EmptyDefaultName = "empty"
local LightDefaultName = "light"

function EditorSystem:Initialize()
  self.selected = nil;
  self.customObjects = {};
  self.allObjects = {};
  self.icons = {};
  self.iconPaths = {};  --保留下icon的路径(upload素材到服务端时，需要上传图片)
  self.faceTriangles = {};
  self.catTriangles = {};
  self.operation = nil;
  self.showGrid = true;  --是否显示地表网格
  self.EditorStatus = defined.EditorStatus.STOP;  --默认正常编辑器模式
  self.ReloadEditorService = false;
  self.showEverything = false;
  self.prefabMode = false;
  --UI 图标
  self:_LoadIcons();
end

function EditorSystem:GetPrefabToShow()
  return self.prefabToShow;
end

function EditorSystem:SetPrefabToShow(path)
  self.prefabToShow = path;
  -- body
end

function EditorSystem:GetBlankImage()
  return self.blankimage;
end

function EditorSystem:Get2DBaseTex()
  return self.base2DTex;
end


--这里修改为创建一个默认的Scene
function EditorSystem:DefaultScene()
  Engine.SceneManager:GetEditScene();
  local editSceneID = Engine.SceneManager:GetEditSceneID();
  self:CreateCameraNode(true); --创建默认相机
  --local defaultLightNode = self:CreateLight("DirectionLight");
  --local lightTrans = defaultLightNode:GetComponent(apolloengine.Node.CT_TRANSFORM);
end


function EditorSystem:SetGameSceneID(sceneID)
  self.gameSceneID = sceneID;
end


function EditorSystem:GetGameScene()
  local scene = Engine.SceneManager:GetPreviewScene();
  if scene == nil then 
    scene = Engine.SceneManager:GetEditScene();
  end
  return scene;
end

function EditorSystem:GetUtilityScene()
  return Engine.SceneManager:Instance():GetUtilityScene();
end



function EditorSystem:GetEditScene()
  local scene = nil;
  scene = Engine.SceneManager:GetEditScene();
  return scene;
end

function EditorSystem:GetEditSceneId()
  return Engine.SceneManager:GetEditSceneID();
end

function EditorSystem:GetUISceneID()
  return Engine.SceneManager:GetUISceneID();
end

--检查相机合法性
function EditorSystem:_GetCameraComponents(node,cameraMap)
  if not venuscore.isNil(node) then
    local cCom = node:GetComponent(Engine.Node.CT_CAMERA);
    if not venuscore.isNil(cCom) and cCom.LayerMask then --过滤掉EditorCamera
      --local layer = cCom.LayerMask;
      --if layer == nil then
      --  local kkk = 3;
      --end
      
      local sequence = cCom.Sequence;
      local isActive = cCom:isActiveHierarchy();
      if isActive == true then
        local key = tostring(sequence) .. tostring(isActive);
        cameraMap[key] = cameraMap[key] or {};
        table.insert(cameraMap[key],cCom);
      end
    end
    local subNodes = node:GetChildrens();
    for k,v in pairs(subNodes) do
      self:_GetCameraComponents(v, cameraMap);
    end
  end
end

function EditorSystem:CheckCameraValidity()
  local scene = self:GetEditScene();
  local cameraMap = {};
  if scene then
    local rootNode = scene:GetRootNode();
    self:_GetCameraComponents(rootNode,cameraMap);
    for key, cameraList in pairs(cameraMap) do
      if #cameraList > 1 then
        return false,key,cameraList;
      end
    end
  end
  return true,nil,nil;
end




--每次调用此函数的时候循环一遍，不在保存id和node的关系了（会导致node删除不掉)
function EditorSystem:GetNodeByID(strID)
  local scene = self:GetEditScene();
  local apolloRootNode = scene:GetRootNode();
  local id2Node = {};
  self:_Traverse(apolloRootNode,id2Node);
  return id2Node[strID];
end

function EditorSystem:_Traverse(rootNode, id2Node)
  local nodeID = (rootNode:GetObjectID());
  id2Node[nodeID] = rootNode;
  local childList = rootNode:GetChildrens();
  local childCnt = #childList;
  for i = 1, childCnt do
    local apolloChildNode = childList[i];
    self:_Traverse(apolloChildNode,id2Node);
  end
end

function EditorSystem:Select(content)
  self.selected = content;
end


function EditorSystem:EnableBrush()
  if self.onBrushMode == false then
    self.onBrushMode = true;
    self.constantSelected = self.selected;
  end
end

function EditorSystem:DisableBrush()
  self.onBrushMode = false;
  self.constantSelected = nil;
end

function EditorSystem:GetSelected()
  if self.onBrushMode then
    return self.constantSelected;
  else
    if Core.isNil(self.selected) then
      self.selected = nil;
    end
    return self.selected;
  end
end

function EditorSystem:GetOperationType()
  return self.operation;
end

function EditorSystem:SetOperationType(operationType)
  self.operation = operationType;
end

function EditorSystem:GetShowGrid()
  return self.showGrid;
end

function EditorSystem:SetShowGrid(isShow)
  self.showGrid = isShow;
end

function EditorSystem:IsPlay()  --是否处于Play状态(PAUSE状态也是PLAY状态)
  return bitop.band(defined.EditorStatus.PLAY, self.EditorStatus) ~= 0;
end

function EditorSystem:IsPause() --是否处于暂停状态
  return (bitop.band(defined.EditorStatus.PLAY,self.EditorStatus) ~= 0) and (bitop.band(defined.EditorStatus.PAUSE, self.EditorStatus) ~= 0);
end

function EditorSystem:IsStop() --是否处于停止状态(正常编辑器状态)
  return self.EditorStatus == defined.EditorStatus.STOP;
end

function EditorSystem:Play()
  self.EditorStatus = defined.EditorStatus.PLAY;
  --将与Play模式互斥的插件卸载掉
  self.cachedPlugins = {};
  local pluginList = PluginSystem:GetPluginList();
  for key,value in pairs(pluginList) do
    local plugin = value;
    local pluginName = value:GetName();
    local needUnload = value:UnloadWhenPlaying();
    local status = value:IsLoaded();
    if status == true and needUnload == true then
      self.cachedPlugins[pluginName] = true;
      value:Unload();
    end
  end
  self:LoadUserScript(); --加载用户脚本
end

function EditorSystem:Pause()
  if self:IsPlay() then
    self.EditorStatus = bitop.bor(defined.EditorStatus.PAUSE, self.EditorStatus);
  end
end

function EditorSystem:ShouldReloadEditorService()
  return self.ReloadEditorService;
end

function EditorSystem:Stop()
  if self:IsPlay() then  --需要重新加载编辑器Service
    --重新加载Run时临时卸载的插件
    local pluginList = PluginSystem:GetPluginList();
    for key,value in pairs(pluginList) do
      local plugin = value;
      local pluginName = value:GetName();
      if self.cachedPlugins[pluginName] == true then
        value:Load();
      end
    end
    self.cachedPlugins = {};
    self.ReloadEditorService = true;
    self:UnloadUserScript();
  end
  self.EditorStatus = defined.EditorStatus.STOP;
end

--加载用户脚本
function EditorSystem:LoadUserScript()
  apollocore.Framework.AddSynchronizeUpdateCallback =
    function(_, user_main, func_name)
      local func_body = user_main[func_name];
        self.update_callback = function()
          func_body(user_main);
        end
    end

  local userMainScript = "scrs:main.lua";
  local fullpath = Core.IFileSystem:PathAssembly(userMainScript);
  self.UserScriptProjFunc = dofile(fullpath);
  self.UserScriptProjFunc:Initialize();
end

function EditorSystem:UnloadUserScript()
  self:Select(nil);
end

--返回icon纹理数组和路径
function EditorSystem:GetIcons()
  return self.icons,self.iconPaths;
end

function EditorSystem:GetEditorCameraOutput()
  local ediorCamera = self:GetEditorCamera();
  local cameraComponent = ediorCamera:GetCameraComponent();
  local rendertarget = cameraComponent:GetAttachedRenderTarget();
  return rendertarget:GetAttachment(Engine.RHIDefine.TA_COLOR_0);
end

function EditorSystem:GetEditorCamera()
  --local editScene = self:GetEditScene()
  local editCamera = Engine.SceneManager:GetEditCamera();
  --local editCamera = editScene:GetEditCamera();
  return editCamera;
end

function EditorSystem:GetGrid()
  local grid = Engine.SceneManager:GetGrid();
  return grid;
end


--得到所有脚本创建的camera
function EditorSystem:GetSceneCameraList()
  local sceneCameraList = {};
  for id, node in pairs(self.allObjects) do
    if node:GetNativeNode():GetComponent(apollocore.Node.CT_CAMERA) ~= nil then
      table.insert(sceneCameraList,node);
    end
  end
  return sceneCameraList;
end

--创建Eitor相机
function EditorSystem:_CreateEditorCamera(size, near, far, pos, lookat, up)
  self.rendertarget = Engine.RenderTargetEntity();--创建一个FBO
  self.rendertarget:PushMetadata(--设置FBO格式
  Engine.RenderTargetMetadata(
    Engine.RenderTargetEntity.TEXTURE_2D,
    Engine.RenderTargetEntity.ST_SWAP_UNIQUE,
      Math.IntVec4(0,0,size.x,size.y),--视口大小
      size));--分辨率
  local depth = self.rendertarget:MakeTextureAttachment(Engine.RenderTargetEntity.TA_DEPTH_STENCIL);
  depth:PushMetadata(
    Engine.DepthRenderBufferMetadata(
      Engine.RenderTargetEntity.ST_SWAP_UNIQUE,
      size,
      Engine.TextureEntity.PF_DEPTH24_STENCIL8
    ));
  self.outputtexture = self.rendertarget:MakeTextureAttachment(Engine.RenderTargetEntity.TA_COLOR_0);--增加color0纹理
  self.outputtexture:PushMetadata(--创建纹理
    Engine.TextureBufferMetadata(size));
  self.rendertarget:CreateResource();
  
  local genericNode = self:GetEditScene():CreateNode(Engine.Node.CT_NODE);
  local trans = genericNode:CreateComponent(Engine.Node.CT_TRANSFORM);
  trans:SetLocalPosition(Math.Vec3(0.0,0.0,0.0));
  genericNode:SetLayer(Engine.LayerMask.MC_MASK_EDITOR_SCENE_LAYER);
  genericNode:SetName(EditorCameraName);  --给一个默认的名字
  local cameraComponent = genericNode:CreateComponent(Engine.Node.CT_CAMERA);
 -- cameraComponent:SetSceneCamera()--场景编辑器摄像机
  cameraComponent:FixedResolution(size);
  cameraComponent:SetLayerMaskEverything();
  cameraComponent:CreatePerspectiveProjection(near,far);
  cameraComponent:CreateRealCameraProjection(near,far);
  cameraComponent:LookAt(pos, lookat, up);
  cameraComponent:AttachRenderTarget(self.rendertarget);
  cameraComponent:Recalculate();
  cameraComponent:Activate();
  cameraComponent:SetClearColor(Engine.Color(0.2, 0.2, 0.2, 1.0));

  return genericNode;
end

--创建所有界面图标
function EditorSystem:_LoadIcons()
  local texTranslate = Engine.TextureEntity();
  texTranslate:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Translate));
  texTranslate:CreateResource();
  self.icons["Translate"] = texTranslate;
  self.iconPaths["Translate"] = defined.Icons.Translate;

  local texRotation = Engine.TextureEntity();
  texRotation:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Rotation));
  texRotation:CreateResource();
  self.icons["Rotation"] = texRotation;
  self.iconPaths["Rotation"] = defined.Icons.Rotation;

  local texScale = Engine.TextureEntity();
  texScale:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Scale));
  texScale:CreateResource();
  self.icons["Scale"] = texScale;
  self.iconPaths["Scale"] = defined.Icons.Scale;

  local texPlay = Engine.TextureEntity();
  texPlay:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Play));
  texPlay:CreateResource();
  self.icons["Play"] = texPlay;
  self.iconPaths["Play"] = defined.Icons.Play;

  local texPause = Engine.TextureEntity();
  texPause:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Pause));
  texPause:CreateResource();
  self.icons["Pause"] = texPause;
  self.iconPaths["Pause"] = defined.Icons.Pause;

  local texStop = Engine.TextureEntity();
  texStop:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Stop));
  texStop:CreateResource();
  self.icons["Stop"] = texStop;
  self.iconPaths["Stop"] = defined.Icons.Stop;

  local texGrid = Engine.TextureEntity();
  texGrid:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Grid));
  texGrid:CreateResource();
  self.icons["Grid"] = texGrid;
  self.iconPaths["Grid"] = defined.Icons.Grid;

  local hierarchy_view = Engine.TextureEntity();
  hierarchy_view:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.Hierarchy_View));
  hierarchy_view:CreateResource();
  self.icons["hierarchy_view"] = hierarchy_view;
  self.iconPaths["hierarchy_view"] = defined.Icons.Hierarchy_View;
  
  --上传素材页面新素材默认图标
  local materialIcon = Engine.TextureEntity();
  materialIcon:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.MaterialDefault));
  materialIcon:CreateResource();
  self.icons["default_material"] = materialIcon;
  self.iconPaths["default_material"] = defined.Icons.MaterialDefault;
  
  --上传素材页面替换图片的图标
  local changepicIcon = Engine.TextureEntity();
  changepicIcon:PushMetadata(Engine.TextureFileMetadata(
		Engine.RHIDefine.TEXTURE_2D,
    Engine.RHIDefine.TU_STATIC,
    Engine.RHIDefine.PF_AUTO,1,true,0,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TW_CLAMP_TO_EDGE,
    Engine.RHIDefine.TF_LINEAR,
    Engine.RHIDefine.TF_LINEAR_MIPMAP_LINEAR,
    defined.Icons.ChangePic));
  changepicIcon:CreateResource();
  self.icons["change_pic"] = changepicIcon;
  self.iconPaths["change_pic"] = defined.Icons.ChangePic;
end

--删除对象
function EditorSystem:Destroy(node)
  local cmd = DestroyNodeCmd(node:GetContentPath())
  CommandManager:DoIt(cmd);
end

--删除对象的整棵树
function EditorSystem:DestroyNodeTree(rootNode)
  self:Destroy(rootNode);
end

function EditorSystem:CreateParticleSystem()
  local cpc = CreateParticleNodeCmd(ParticleDefaultName);
  local particleNode = CommandManager:DoIt(cpc);
  return particleNode;
end

function EditorSystem:CreateNewParticle()
  local cnpc = CreateNewParticleNodeCmd("newparticle");
  local particleNode = CommandManager:DoIt(cnpc);
  return particleNode;
end

function EditorSystem:CreateBodySignal()
  local cmd = CreateBodySignalCmd();
  local bodyNode = CommandManager:DoIt(cmd);
  return bodyNode;
end

function EditorSystem:CreateBodySignalMid()
  local cmd = CreateBodySignalMidCmd();
  local bodyNode = CommandManager:DoIt(cmd);
  return bodyNode;
end

function EditorSystem:CreateBodySignalLow()
  local cmd = CreateBodySignalLowCmd();
  local bodyNode = CommandManager:DoIt(cmd);
  return bodyNode;
end

function EditorSystem:CreateBodyAttacher()
  local cmd = CreateBodyAttacherCmd();
  local bodyNode = CommandManager:DoIt(cmd);
  return bodyNode;
end


function EditorSystem:CreateSpineNode(skeleton, atlaspath)
  local csc = CreateSpineNodeCmd(skeleton, atlaspath, defined.DefaultSpineMaterial);
  local spineNode = CommandManager:DoIt(csc);
  return spineNode;
end

function EditorSystem:CreateTextNode(fontPath)
  local csc = CreateTextNodeCmd(fontPath);
  local textNode = CommandManager:DoIt(csc);
  return textNode;
end

function EditorSystem:CreateSDFTextNode(fntPath, pngPath)
  local csc = CreateSDFTextNodeCmd(fntPath, pngPath);
  local sdfTextNode = CommandManager:DoIt(csc);
  return sdfTextNode;
end

function EditorSystem:CreateCameraNode(isMainCamera)
  
end

function EditorSystem:_RenameNodeName(node)
  local name = node:GetName()
  if name == nil then
    return
  end

  local index = name:match'^.*()/'
  if index ~= nil then
    local sub =  string.sub(name, index + 1)
    local dst = sub:gsub("%..*", "")
    node:SetName(dst)
  end
end

function EditorSystem:GetStandardHeadZval(UVcoord)
  if UVcoord == nil then
    return nil;
  else
    local zNorm = self.HeadZmapStream:ReadPixel(UVcoord.x*1000 +1, UVcoord.y*1500 + 1);
    if zNorm[1] then
      return zNorm[1]*defined.StandardHead_max;
    else
      return 0;
    end
  end
end


--创建出来的几何体放在哪里呢？
function EditorSystem:CreateBasicObject(baseType)
  local filepath = defined.BasicObjects[baseType];
  local objOptions = defined.BasicObjectOptions[baseType];
  filepath = venuscore.IFileSystem:PathAssembly(filepath);
  local cmd = DeserializeCmd(filepath);
  local baseobject = CommandManager:DoIt(cmd);
  for i = 1, #baseobject do
    local nativeNode = baseobject[i];
    if nativeNode.GetNativeNode then
      nativeNode = nativeNode:GetNativeNode();
    end
    if objOptions then
      --cull
      local renderComponent = nativeNode:GetComponent(apollocore.Node.CT_RENDER);
      if renderComponent then
        local bCull = objOptions.Cull;
        if bCull == true then
          renderComponent:SetRenderProperty(apollocore.RenderComponent.RP_CULL);
        elseif bCull == false then
          renderComponent:EraseRenderProperty(apollocore.RenderComponent.RP_CULL);
        end
      end
    end
    self:_RenameNodeName(nativeNode)
  end
  local headNode = baseobject[#baseobject];
  if headNode.GetNativeNode then
    headNode = headNode:GetNativeNode();
  end
  return headNode;
end

function EditorSystem:CreateQuad(baseType)
end


function EditorSystem:CreateEmptyNode(nodeType)
  
end

function EditorSystem:CreateAudioNode(nodeType)
  
end

function EditorSystem:CreateLight(lightType) --"AmbientLight" "DirectionLight" "PointLight" "SpotLight"
  
end

function EditorSystem:CreateObjectFrombundle(path)
  path = Core.IFileSystem:Instance():PathAssembly(path);
  --[[local jsonpath = path..".meta";
  local rootconfig = venusjson.LaodJsonFile(jsonpath);
  if rootconfig then
    Engine.Framework:SetCurrentAdvancedShading(rootconfig.AdvancedShading == true);
    Engine.Framework:SetEditorAdvancedShading(rootconfig.AdvancedShading == true);
  end]]
  local dSceneCmd = DeserializeSceneCmd.new(path);
  local scene = CommandManager:DoIt(dSceneCmd);
 
  --[[local cmd = DeserializeCmd(path);
  local baseobject = CommandManager:DoIt(cmd);
  if baseobject[#baseobject].GetNativeNode then
    return baseobject[#baseobject]:GetNativeNode();
  else
    return baseobject[#baseobject];
  end
  ]]
end

--创建出的node有的需要更新
function EditorSystem:Update(def)

end



function EditorSystem:RegisterAssetsPathChange(caller, func)
  self.OnAssetsPathChangeCaller = caller;
  self.OnAssetsPathChangeFunc = func;
end

function EditorSystem:SetAssetsPath(path, callback)
  if self:GetPrefabMode() then
    return;
  end
  SystemDirectories["scene_asset"] = path;
  if self.OnAssetsPathChangeFunc then
    self.OnAssetsPathChangeFunc(self.OnAssetsPathChangeCaller, callback);
  end

  local _, sEnd = string.find(path,SystemDirectories["assets"],1,true);
  if sEnd ~= nil then
    local temp = string.sub(path,sEnd + 1);
    SystemDirectories["scene_library"] = SystemDirectories["library"] .. temp;
  end
end

function EditorSystem:OpenScene(file, callback)

  local callbackFunc = nil;
  if callback ~= nil then
    callbackFunc = function()
      self.sceneFile = file;
      self:CreateObjectFrombundle(file);
      callback();
    end;
  end
  local metadata = filelfs:GetMetaData(file);
  local parent = filelfs:GetParentDirectory(file);
  local filename = filelfs:getFileNameWithPosfix(file);
  local name = filelfs:getFileName(filename);
  local n = string.len(name);
  local folder = string.sub(string.sub(parent, -n-1), 1, n);
  self:SetAssetsPath(filelfs:ConverToRelativePath(parent), callbackFunc);
  if callback == nil then
    if file ~= defined.PrefabScene then
      self.sceneFile = file;
    end
    self:CreateObjectFrombundle(file);
  end
end


function EditorSystem:GetCurrentScenePath()
  return self.sceneFile;
end

function EditorSystem:CreateScene(path, name)
  Engine.SceneManager:ScrCreateScene(defined.EditorSceneName); --创建一个新的场景
  self:DefaultScene(); --默认创建一个场景摄像机
  filelfs:CreateDirectory(path);
  self.sceneFile = path .. name .. '.scene';
  BundleManager:SaveScene(self.sceneFile);
  self:SetAssetsPath(filelfs:ConverToRelativePath(path));
end

function EditorSystem:ClearScene()
  self:Select(nil);
  Engine.SceneManager:ClearScene();  --清理场景
  --apolloengine.Framework:ForceClear();
  collectgarbage();
end

function EditorSystem:SyncStaticID(gameRoot, sceneRoot)
  gameRoot:SetStaticID(sceneRoot:GetStaticID());
  local compTypeList = gameRoot:GetComponentTypes();
  for j = 1, #compTypeList do
    local comType = compTypeList[j];
    local gameComp = gameRoot:GetComponent(comType);
    local sceneComp = sceneRoot:GetComponent(comType);
    if gameComp then
      if comType == apollocore.Node.CT_SCRIPT or comType == apollocore.Node.CT_BLUEPRINT then
        local gameInstance = gameComp.Instances;
        local sceneInstance = sceneComp.Instances;
        for key,value in pairs(gameInstance) do
          value._hostID = sceneComp:GetContentPath();
          sceneInstance[key]._hostID = sceneComp:GetContentPath();
        end
        gameComp:SetStaticID(sceneComp:GetStaticID());
      else
        gameComp:SetStaticID(sceneComp:GetStaticID());
      end
    end
  end

  local gameChildrens = gameRoot:GetChildrens();
  local gameChildrensMap = {}
  for _,subNode in pairs(gameChildrens) do
    --过滤掉辅助体
    local nodeShow = not subNode:isLayer(apollocore.LayerMask.MC_MASK_EDITOR_SCENE_LAYER)
            and not subNode:isLayer(apollocore.LayerMask.MC_MASK_EDITOR_UI_LAYER)
            and not subNode:isLayer(apollocore.LayerMask.MC_MASK_EDITOR_CLOTHDEBUG_LAYER);
    if nodeShow then
      assert(gameChildrensMap[subNode:GetSubNodeKey()] == nil, "[DeserializeCmd Find Repeated nodeKey In Same Level Of Game!!]");
      gameChildrensMap[subNode:GetSubNodeKey()] = subNode;
    end
  end

  local sceneChildrens = sceneRoot:GetChildrens();
  local sceneChildrensMap = {}
  for _,subNode in pairs(sceneChildrens) do
    local nodeShow = not subNode:isLayer(apollocore.LayerMask.MC_MASK_EDITOR_SCENE_LAYER)
            and not subNode:isLayer(apollocore.LayerMask.MC_MASK_EDITOR_UI_LAYER)
            and not subNode:isLayer(apollocore.LayerMask.MC_MASK_EDITOR_CLOTHDEBUG_LAYER);
    if nodeShow then
      assert(sceneChildrensMap[subNode:GetSubNodeKey()] == nil, "[DeserializeCmd Find Repeated nodeKey In Same Level Of Scene!!]");
      sceneChildrensMap[subNode:GetSubNodeKey()] = subNode;
    end
  end

  for nodeKey,node in pairs(gameChildrensMap) do
    self:SyncStaticID(node, sceneChildrensMap[nodeKey])
  end

end



function EditorSystem:ResetGameScene()
  Engine.SceneManager:ClearSyncScene(Engine.SceneManager:GetPreviewScene());
  Engine.SceneManager:DeleteGameScene(); --game场景全部删除
end


function EditorSystem:ShowEverything()
  return self.showEverything;
end

function EditorSystem:SetShowStaus(status)
  self.showEverything = status;
end

function EditorSystem:SetPrefabMode(flag)
  self.prefabMode = flag;
end

function EditorSystem:GetPrefabMode()
  return self.prefabMode;
end

function EditorSystem:GetEditPrefab()
  return self.currentEditPrefabNode;
end

function EditorSystem:SyncRootContentPath()
  -- 同步rootNode的ContentPath
  local sceneEdit = EditorSystem:GetEditScene()
  local rootNodeEdit = sceneEdit:GetRootNode();
  local sceneGame = EditorSystem:GetGameScene()
  local rootNodeGame = sceneGame:GetRootNode();
  local cPath = rootNodeEdit:GetStaticID();
  rootNodeGame:SetStaticID(cPath);
end

return EditorSystem;
