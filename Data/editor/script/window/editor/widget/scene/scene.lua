local imguifunction = require "imguifunction"
local Math = require "Math"
local Engine = require "Engine"
local Core = require "MeteorCore"
local Object = require("MeteorCore.core.Object")
local editordefined = require "window.editor.system.defined"
local EditSystem = require "window.editor.system.editsystem"
local filelfs = require "window.editor.widget.assets.fileutility"


local ImGui = imguifunction.ImGui;

local scene = class("scene", Object)


function scene:ctor()
  self.mousespeed = editordefined.MouseSpeed;
  self.wheelspeed = editordefined.WheelSpeed;
  self.keyspeed = editordefined.KeyBoardSpeed;
  self.dragthreshold = editordefined.MouseDragThreshold;
  self.keypressrate = editordefined.KeyBoardPressRate;
  self.keypressdelay = editordefined.KeyBoardPressDelay;

  --[[local scene = EditSystem:GetEditScene()
  scene:CreateDefaultRenderTarget(Math.IntVec2(128,128));
  local defaultRenderTarget = scene:GetDefaultRenderTarget();]]--

  
  --地面网格
  self.isGridShow = true;
  
  self.titleName = "Scene";
  self.isOnGui = true;


end

function scene:ReleaseConvertion()
  self.convertTex = nil;
  self.srgb2normal_rt = nil;
  local scene = EditSystem:GetEditScene();
  if self.srgb2normal_camera ~= nil then
    scene:DeleteNode(self.srgb2normal_camera:GetContentPath());
    self.srgb2normal_camera = nil;
  end  
  if self.quadNode ~= nil then
    scene:DeleteNode(self.quadNode:GetContentPath());
    self.quadNode = nil;
  end
  self.quadrender = nil;
end

function scene:OnGui(timespan)
  if self.isOnGui then
    ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowPadding, imguifunction.ImVec2(0,0));
    local b;
    local dockFlag = ImGui.ImGuiWindowFlags_NoScrollbar + ImGui.ImGuiWindowFlags_NoCollapse
    b,self.isOnGui = ImGui.Begin(self.titleName, true, dockFlag);
    self:ProcessInput();
    
    --显示隐藏Grid
    local gridNode = EditSystem:GetGrid();
    self:SetGridShow(gridNode, EditSystem:GetShowGrid());
    
    local size = ImGui.GetWindowSize();
    local h = ImGui.GetFrameHeight();
    size.y = size.y - h;

    local editorCamera = EditSystem:GetEditorCamera();
    local cameraComponent = editorCamera:GetCameraComponent();
    cameraComponent:ChangeResolution(Math.IntVec2(size.x, size.y));
   
    local editorOutput = EditSystem:GetEditorCameraOutput();
    local utilityScene = EditSystem:GetUtilityScene();
    
    self:_UpdateGrid(gridNode, cameraComponent);

    self:_ProcessDragDropTarget(); --处理拖拽

    local isSceneShow2D = false;
    if EditSystem:GetPrefabMode() then
      --prefab编辑器模式下暂时禁用2D场景，有点乱
    else
      ImGui.SetCursorPos(imguifunction.ImVec2(size.x*0.01,h+size.y*0.01));
      --只能开一个子窗口放按钮 否则无法区分拾取点击和按钮点击
      ImGui.BeginChild("child", imguifunction.ImVec2(90, 25));--实践值...
      local viewstring = isSceneShow2D and "View3DScene" or "View2DScene";
      if ImGui.Button(viewstring) then
        EditSystem:SetSceneShow2D(not isSceneShow2D);
      end
      ImGui.EndChild();
    end

    ImGui.End();
    ImGui.PopStyleVar();
  end
end

function scene:ProcessInput()
  local editorCamera = EditSystem:GetEditorCamera();
  if Core.isNil(editorCamera) then
    --local editorCamera = EditSystem:GetEditorCamera();
    return;
  end
  
  local cameraComponent = editorCamera:GetCameraComponent();
  if ImGui.IsWindowHovered() then
    if ImGui.IsKeyDown(Core.InputPropery.KB_KEY_LEFT_ALT) or ImGui.IsKeyDown(Core.InputPropery.KB_KEY_RIGHT_ALT) then
      if ImGui.IsMouseDragging(editordefined.Mouse.Left,self.dragthreshold) then
        local v2 = ImGui.GetMouseDragDelta(editordefined.Mouse.Left,self.dragthreshold) / 180 * math.pi * self.mousespeed;
        local up = cameraComponent:GetUp();
        local rotatepoint = cameraComponent:GetPosition() + cameraComponent:GetForward() * 1;
        local vec = cameraComponent:GetPosition() - rotatepoint;
        local right = up:Cross(vec);
        local rot = Math.Quaternion();
        rot:RotateAxis(right, -v2.y);
        vec = vec * rot;
        if up:Dot(Math.Vec3(0,1,0)) < 0 then
          up = Math.Vec3(0,-1,0);
        else
          up = Math.Vec3(0,1,0);
        end
        rot:RotateAxis(up,-v2.x);
        vec = vec * rot;
        right = right * rot;
        up = vec:Cross(right);
        cameraComponent:LookAt(rotatepoint + vec, rotatepoint, up);
        cameraComponent:Recalculate();
        ImGui.ResetMouseDragDelta(editordefined.Mouse.Left);
      end
      return;
    end
    --缩放
    local vertical,horizontal = ImGui.GetMouseWheel();
    if vertical ~= 0 then
      cameraComponent:SetPosition(cameraComponent:GetPosition() + cameraComponent:GetForward() * vertical * self.wheelspeed);
    end
    --旋转
    if ImGui.IsMouseDragging(editordefined.Mouse.Right,self.dragthreshold) then
      local v2 = ImGui.GetMouseDragDelta(editordefined.Mouse.Right,self.dragthreshold) / 180 * math.pi * self.mousespeed;
      local position = cameraComponent:GetPosition();
      local forward = cameraComponent:GetForward();
      local up = cameraComponent:GetUp();
      local right = forward:Cross(up);
      local xrot =  Math.Quaternion();
      xrot:RotateAxis(right, -v2.y);
      forward = forward * xrot;
      if up:Dot(Math.Vec3(0,1,0)) < 0 then
        up = Math.Vec3(0,-1,0);
      else
        up = Math.Vec3(0,1,0);
      end
      xrot:RotateAxis(up, -v2.x);
      forward = forward * xrot;
      right = right * xrot;
      up = right:Cross(forward);
      local lookatpos = position + forward;
      cameraComponent:LookAt(position, lookatpos, up);
      cameraComponent:Recalculate();
      ImGui.ResetMouseDragDelta(editordefined.Mouse.Right);
    end
    --左键平移
    if ImGui.IsMouseDragging(editordefined.Mouse.Mid,self.dragthreshold) then
      local v2 = ImGui.GetMouseDragDelta(editordefined.Mouse.Mid,self.dragthreshold) * self.mousespeed / 25;
      local position = cameraComponent:GetPosition();
      local forward = cameraComponent:GetForward();
      local up = cameraComponent:GetUp();
      local right = forward:Cross(up);
      up:NormalizeSelf();
      right:NormalizeSelf();
      position = position + right * -v2.x + up * v2.y;
      cameraComponent:SetPosition(position);
      ImGui.ResetMouseDragDelta(editordefined.Mouse.Mid);
    end
    --平移
    if ImGui.IsMouseDown(editordefined.Mouse.Right) then
      local forward = cameraComponent:GetForward();
      local up = cameraComponent:GetUp();
      local right = forward:Cross(up);
      forward:NormalizeSelf();
      right:NormalizeSelf();
      if ImGui.IsKeyDown(Core.InputPropery.KB_KEY_W) then
        cameraComponent:SetPosition(cameraComponent:GetPosition()
                + forward * ImGui.GetKeyPressedAmount(Core.InputPropery.KB_KEY_W,self.keypressdelay,self.keypressrate) * self.keyspeed);
      end
      if ImGui.IsKeyDown(Core.InputPropery.KB_KEY_S) then
        cameraComponent:SetPosition(cameraComponent:GetPosition()
                - forward * ImGui.GetKeyPressedAmount(Core.InputPropery.KB_KEY_S,self.keypressdelay,self.keypressrate) * self.keyspeed);
      end
      if ImGui.IsKeyDown(Core.InputPropery.KB_KEY_A) then
        cameraComponent:SetPosition(cameraComponent:GetPosition()
                - right * ImGui.GetKeyPressedAmount(Core.InputPropery.KB_KEY_A,self.keypressdelay,self.keypressrate) * self.keyspeed);
      end
      if ImGui.IsKeyDown(Core.InputPropery.KB_KEY_D) then
        cameraComponent:SetPosition(cameraComponent:GetPosition()
                + right * ImGui.GetKeyPressedAmount(Core.InputPropery.KB_KEY_D,self.keypressdelay,self.keypressrate) * self.keyspeed);
      end
      if ImGui.IsKeyDown(Core.InputPropery.KB_KEY_Q) then
        cameraComponent:SetPosition(cameraComponent:GetPosition()
                - up * ImGui.GetKeyPressedAmount(Core.InputPropery.KB_KEY_Q,self.keypressdelay,self.keypressrate) * self.keyspeed);
      end
      if ImGui.IsKeyDown(Core.InputPropery.KB_KEY_E) then
        cameraComponent:SetPosition(cameraComponent:GetPosition()
                + up * ImGui.GetKeyPressedAmount(Core.InputPropery.KB_KEY_E,self.keypressdelay,self.keypressrate) * self.keyspeed);
      end
    end
  end
end

function scene:_ProcessDragDropTarget()
  --scene窗口拖拽创建的node默认挂到根节点，不需要再执行AttachCmd
  if imguifunction.ImGui.BeginDragDropTarget() then
    if imguifunction.ImGui.IsDragDropPayloadBeingAccepted() then
      imguifunction.ImGui.SetMouseCursor(imguifunction.ImGui.ImGuiMouseCursor_Hand);
    else
      imguifunction.ImGui.SetMouseCursor(imguifunction.ImGui.ImGuiMouseCursor_COUNT); --禁止拖拽
    end
    local payloadAtapter = imguifunction.ImGui.AcceptDragDropPayload("assets");
    if payloadAtapter:IsEmpty() == false then
      local bundlenode = nil;
      local filepath = payloadAtapter:GetData();
      local bundlePath = filepath;
      local filetype = filelfs:getExtension(filepath);
      if filetype == "bundle" then
        bundlenode = EditSystem:CreateObjectFrombundle(filepath);
      elseif filetype ~= nil and string.lower(filetype) == "fbx" then
        bundlePath = filelfs:GetFbxbundle(filepath);
        if filelfs:exists(bundlePath) then
          bundlenode = EditSystem:CreateObjectFrombundle(bundlePath);
        end
      elseif filetype ~= nil and string.lower(filetype) == "obj" then
        bundlePath = filelfs:GetObjbundle(filepath);
        if filelfs:exists(bundlePath) then
          bundlenode = EditSystem:CreateObjectFrombundle(bundlePath);
        end
      elseif filetype == "prefab" then
        local isAllowCreate = true;
        if EditSystem:GetPrefabMode() then
          local currentPath = EditSystem:GetEditPrefab().PrefabPath;
          local insertPrefabPath = filelfs:ConverToRelativePath(filepath);
          if currentPath == insertPrefabPath then
            isAllowCreate = false; --不允许prefab编辑的时候循环嵌套自身
          end
        end
        if isAllowCreate then
          bundlenode = EditSystem:CreateObjectFrombundle(filepath);
          if bundlenode then
            if EditSystem:GetPrefabMode() then
              local cmd1 = SetHostPrefabPathCmd(bundlenode:GetContentPath(), EditSystem:GetEditPrefab().PrefabPath);
              CommandManager:DoIt(cmd1);
              local cmd = AttachNodeCmd(EditSystem.currentEditPrefabNode:GetContentPath(), bundlenode:GetContentPath());
              CommandManager:DoIt(cmd);
            end       
          end
        end
      end
      if bundlenode then
        EditSystem:Select(bundlenode);
        --编辑器创建node，需要检测下是否与同层级node名字重复
        local name = EditorCheckFunc:GenerateNonRepeatName(bundlenode, bundlenode.Name);
        local cmd = PropertyChangeCmd(bundlenode:GetContentPath(),"Name", name);
        CommandManager:DoIt(cmd);
        local cmd = SetPrefabPathCmd(bundlenode:GetContentPath(), filelfs:ConverToRelativePath(bundlePath));
        CommandManager:DoIt(cmd);
      end
    end
    imguifunction.ImGui.EndDragDropTarget();
  end
end

function scene:GetTitleName()
  return self.titleName;
end

function scene:IsOnGui()
  return self.isOnGui;
end

function scene:SetOnGui(isOnGui)
  self.isOnGui = isOnGui;
end

function scene:SetGridShow(gridNode, isShow)
  if self.isGridShow ~= isShow then
    local renderComponent = gridNode:GetRenderComponent();
    self.isGridShow = isShow;
    if isShow then
      renderComponent:SetRenderProperty(Engine.GraphicDefine.RP_SHOW);
    else
      renderComponent:EraseRenderProperty(Engine.GraphicDefine.RP_SHOW);
    end  
  end
end

function scene:_UpdateGrid(gridNode, cameraComponent)
  local transformComponent = gridNode:GetTransformComponent();
  local possub = cameraComponent:GetPosition();

  local offsetx;
  if(possub.x>0) then
     offsetx = math.floor(possub.x);
  else
     offsetx = -math.floor(-possub.x);
  end
  local offsetz; 
  if(possub.z>0) then
     offsetz = math.floor(possub.z);
  else
     offsetz = -math.floor(-possub.z);
  end
  if(offsetz ~= 0 or offsetx ~= 0) then
    transformComponent:SetLocalPosition(Math.Vec3(offsetx,0.0,offsetz));
  end
end


return scene;
