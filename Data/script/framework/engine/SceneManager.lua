--[[
    desc: cclass SceneManager wrap
    author:{hjy}
    time:2022-09-28 19:00:25
]]

require("functions")

local Math = require("Math")
local Engine = require("Engine")
require("framework.engine.Scene")
require("framework.engine.GObject")
local SceneSequence = require("framework.engine.SceneSequence")

function Engine.SceneManager:ScrCreateScene(sceneName)
  local scene = Engine.SceneManager:Instance():CreateScene(sceneName)
  self:InitScene(scene);
  return scene
end

function Engine.SceneManager:InitScene(scene)
  if _EDITOR then
    scene:Init();
    Engine.SceneManager.EditCamera = scene:CreateEditorCamera();
    --Engine.SceneManager.EditCamera2D = scene:CreateEditorCamera2D();
    --Engine.SceneManager.FaceEditorCamera = scene:CreateFaceEditorCamera();
    Engine.SceneManager.Grid = scene:CreateGrid();
    Engine.SceneManager.EditSceneID = scene:GetStaticID();
    scene:SetEditorCamera(Engine.ToComCamera(Engine.SceneManager.EditCamera:GetComponent("CameraComponent")));
  end
end

if _EDITOR then
  Engine.SceneManager.PreviewSceneID = nil;
  Engine.SceneManager.EditSceneID = nil;
  Engine.SceneManager.EditorUIID = nil;
  Engine.SceneManager.EditCamera = nil;
  Engine.SceneManager.EditCamera2D = nil;
  Engine.SceneManager.FaceEditorCamera = nil;
  Engine.SceneManager.Grid = nil;
  Engine.SceneManager.SyncSceneList = {};
  
  function Engine.SceneManager:GetEditCamera()
    return Engine.SceneManager.EditCamera;
  end
  
  function Engine.SceneManager:GetEditCamera2D()
    return Engine.SceneManager.EditCamera2D;
  end
  
  function Engine.SceneManager:GetFaceEditorCamera()
    return Engine.SceneManager.FaceEditorCamera;
  end
  
  function Engine.SceneManager:GetGrid()
    return Engine.SceneManager.Grid;
  end
  
  function Engine.SceneManager:CreatePreviewScene(sceneName)
    local previewScene = Engine.SceneManager:CreateScene(sceneName);
    previewScene:SetSequence(SceneSequence.MID);
    Engine.SceneManager.PreviewSceneID = previewScene:GetStaticID();
    return previewScene;
  end
  
  function Engine.SceneManager:GetPreviewScene()
    local previewScene = nil;
    if Engine.SceneManager.PreviewSceneID then
      previewScene = Engine.SceneManager:Instance():GetScenebyId(Engine.SceneManager.PreviewSceneID);
    end
    return previewScene;
  end
  
  
  function Engine.SceneManager:GetEditScene()
    local scene = nil;
    if Engine.SceneManager.EditSceneID then
      scene = Engine.SceneManager:Instance():GetScenebyId(Engine.SceneManager.EditSceneID);
    end
    return scene;
  end
  
  function Engine.SceneManager:GetEditSceneID()
    return Engine.SceneManager.EditSceneID;
  end
  
  function Engine.SceneManager:GetUISceneID()
    if Engine.SceneManager.EditorUIID == nil then
      local scenes = Engine.SceneManager:Instance():GetAllScenes(self);
      for k,v in pairs(scenes) do
        local name = v:GetName();
        if name == "EditorUI" then
          Engine.SceneManager.EditorUIID = v:GetStaticID();
        end
      end
    end
    return Engine.SceneManager.EditorUIID;
  end
  
  --????????????????????????Scene
  function Engine.SceneManager:ClearScene()
    --??????EditorScene
    local editScene = Engine.SceneManager:Instance():GetSceneByID(self, Engine.SceneManager.EditSceneID);
    if editScene then
      Engine.SceneManager.EditCamera = nil;
      Engine.SceneManager.EditCamera2D = nil;
      Engine.SceneManager.Grid = nil;
  
      -- DeleteEditScene
      editScene:SetMainCamera(nil);
      local rootNode = editScene:GetRootNode();
      local childNodes = rootNode:GetChildrens();
      for i = 1, #childNodes do
        editScene:DeleteNode(childNodes[i]);
      end
      local compList = rootNode.Components;
      if compList then
        for key,value in pairs(compList) do
          value:Destroy();
        end
      end
  
      Engine.SceneManager:Instance():DeleteSceneByID(self, Engine.SceneManager.EditSceneID);
      --????????????Scene???Node(?????????Scene????????????)
      if Engine.SceneManager.PreviewSceneID then
        local preViewScene = Engine.SceneManager:Instance():GetSceneByID(self, Engine.SceneManager.PreviewSceneID);
        if preViewScene then
  
          -- DeleteGameScene
          preViewScene:SetMainCamera(nil); --?????????????????????
          rootNode = preViewScene:GetRootNode();
          childNodes = rootNode:GetChildrens(); --???root??????node????????????
          for i = 1, #childNodes do
            preViewScene:DeleteNode(childNodes[i]);
          end
          compList = rootNode.Components;
          --if compList then
          --  for key,value in pairs(compList) do
          --    value:Destroy();
          --  end
          --end
          if compList then -- Destroy() do not detach component from node
            for key, value in pairs(compList) do
              rootNode:DeleteComponent(key); -- it will call comp:Destroy too
            end
          end
  
        end
      end
    end
    Engine.SceneManager.referenceinfos = {};
  end
  
  --??????????????????????????????????????????????????????
  function Engine.SceneManager:AddSyncScene(scene)
    if scene then
      local sceneID = scene:GetStaticID();
      if Engine.SceneManager.SyncSceneList[sceneID] == nil then
        Engine.SceneManager.SyncSceneList[sceneID] = scene;
      end
    end
  end
  
  --????????????game??????????????????????????????????????????????????????
  function Engine.SceneManager:ClearSyncScene(scene)
    local sceneID = scene:GetStaticID();
    Engine.SceneManager.SyncSceneList[sceneID] = nil;
  end
  
  
  function Engine.SceneManager:GetSyncSceneList()
    return Engine.SceneManager.SyncSceneList;
  end
  
  
  function Engine.SceneManager:DeleteEditScene()
    --??????EditorScene
    local editScene = Engine.SceneManager:Instance():GetSceneByID(self, Engine.SceneManager.EditSceneID);
    if editScene then
      Engine.SceneManager.EditCamera = nil;
      Engine.SceneManager.EditCamera2D = nil;
      Engine.SceneManager.Grid = nil;
  
      editScene:SetMainCamera(nil); --?????????????????????
      local rootNode = editScene:GetRootNode();
      --???root??????node????????????
      local childNodes = rootNode:GetChildrens();
      for i = 1, #childNodes do
        editScene:DeleteNode(childNodes[i]);
      end
      -- rootNode?????????scene DetachNode, ?????????????????????rootnode??????compoent
      local compList = rootNode.Components;
      if compList then
        for key,value in pairs(compList) do
          value:Destroy();
          end
      end
  
      Engine.SceneManager:Instance():DeleteSceneByID(self, Engine.SceneManager.EditSceneID);
    end
    Engine.SceneManager.referenceinfos = {};
  end
  
  function Engine.SceneManager:DeleteGameScene()
    if Engine.SceneManager.PreviewSceneID then
      local preViewScene = Engine.SceneManager:Instance():GetSceneByID(self, Engine.SceneManager.PreviewSceneID);
      if preViewScene then
        preViewScene:SetMainCamera(nil); --?????????????????????
        local rootNode = preViewScene:GetRootNode();
        --???root??????node????????????
        local childNodes = rootNode:GetChildrens();
        for i = 1, #childNodes do
          preViewScene:DeleteNode(childNodes[i]);
        end
        local compList = rootNode.Components;
        if compList then
          for key,value in pairs(compList) do
            value:Destroy();
          end
        end
  
      end
      Engine.SceneManager:Instance():DeleteSceneByID(self, Engine.SceneManager.PreviewSceneID);
    end
  end
  
  --????????????GetOrCreate??????GetPreviewScene
  Engine.SceneManager.GetOrCreateScene = Engine.SceneManager.GetPreviewScene;
end
  