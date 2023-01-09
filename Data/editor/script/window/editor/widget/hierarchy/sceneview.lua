local EditorSystem = require "window.editor.system.editsystem"
local imguifunction = require "imguifunction"
local ImGui = imguifunction.ImGui
local Math = require "Math"
local Engine = require "Engine"
local MeteorCore = require "MeteorCore"
local Object = require("MeteorCore.core.Object")
local baseview = require "window.editor.widget.hierarchy.base_view"
local defined = require "window.editor.system.defined"
local panelutility = require "window.editor.widget.inspector.panels.utility" 
local filelfs = require "window.editor.widget.assets.fileutility"
local bit = require("bit")

local sceneview = class("sceneview", baseview)

function sceneview:ctor()
  self.super.new(self);
  self.dragIllegal = nil;
  self.legalName = true;
end

local function SearchComponent(components, node, type, nodeName)
  if nodeName == nil or node.Name == nodeName then
    local comp = node:GetComponent(type)
    if comp ~= nil then
      table.insert(components, comp)
    end
  end
  local subNodes = node:GetChildrens()
  for _, subNode in ipairs(subNodes) do
    SearchComponent(components, subNode, type, nodeName)
  end
end

function sceneview:ShowDeletePrompt()
  if self.ShowDeletePromptDialog == false then
    return;
  end
  
  local node = self.DeleteNode;
  if node then
    local camComp = node:GetComponent(apolloengine.Node.CT_CAMERA);
    if camComp and self._scene:GetMainCamera() then
      if camComp:GetObjectID() == self._scene:GetMainCamera():GetObjectID() then
        local windowFlags = bit.bor(imguifunction.ImGui.ImGuiWindowFlags_NoScrollbar,imguifunction.ImGui.ImGuiWindowFlags_NoResize,imguifunction.ImGui.ImGuiWindowFlags_NoSavedSettings,imguifunction.ImGui.ImGuiWindowFlags_NoDocking);
        imguifunction.ImGui.SetNextWindowSize(imguifunction.ImVec2(0,0));
        local b1,show = imguifunction.ImGui.Begin("Warning", true, windowFlags);
        imguifunction.ImGui.Text("Main camera will be deletetd?\n If select ok,you shoule select a new camera for scene!");
        local contentRegionAvail = imguifunction.ImGui.GetContentRegionAvail();
        local contentRegionAvailWidth = contentRegionAvail.x;
        local contentRegionAvailHeight = contentRegionAvail.y;
        local currentCursorPos = imguifunction.ImGui.GetCursorPos();
        imguifunction.ImGui.SetCursorPos(imguifunction.ImVec2(currentCursorPos.x + contentRegionAvailWidth /2.5,currentCursorPos.y + contentRegionAvailHeight / 2));
        if imguifunction.ImGui.Button("Ok ") then
          self.DeleteCallback();
          self.ShowDeletePromptDialog = false;
          self._scene:SetMainCamera(nil);
          local gameScene = EditorSystem:GetGameScene();
          if gameScene then
            gameScene:SetMainCamera(nil);
          end
        end
        imguifunction.ImGui.SameLine();
        if imguifunction.ImGui.Button("Cancle") then
          self.ShowDeletePromptDialog = false;
        end
        imguifunction.ImGui.End();
      else
        self.DeleteCallback();
        self.ShowDeletePromptDialog = false;
      end
    else
      self.DeleteCallback();
      self.ShowDeletePromptDialog = false;
    end
  end
end

--右键菜单逻辑
function sceneview:_CheckContexMenu(node)
  local action = defined.ContextAction.None;
  if node then --在Node上触发鼠标右键
    imguifunction.ImGui.PushStyleVar(imguifunction.ImGui.ImGuiStyleVar_WindowPadding, imguifunction.ImVec2(8, 8))
    if imguifunction.ImGui.BeginPopupContextItem("Entity Context Menu",1) then
      EditorSystem:Select(node);
      if imguifunction.ImGui.MenuItem("Rename","F2",false,true) then  --重命名
        action = defined.ContextAction.Rename;
      end
      --local scene = EditorSystem:GetEditScene();
      if (node._Script or node:GetObjectID() ~= self._scene:GetRootNode():GetObjectID()) then
        if imguifunction.ImGui.MenuItem("Delete","DEL",false,true) then --删除(如何删除?)
          self:DeletePrompt(node,
            function()
              if(Drag:GetTheObjectID() == node:GetObjectID()) then --要删除的节点和当前选中的节点是同一个节点
                Drag:DropObject();
                EditorSystem:Select(nil);
              end
              local liquify = node:GetComponent(apolloengine.Node.CT_FACELIQUIFY);
              if liquify then
                local liquifyNum = 6;  -- 可创建的人脸液化数量的最大值
                local state = faceliquifyManager:GetLiquifyPointState(node);
                faceliquifyManager:ClearShaderLens(node:GetRoot(),state["index"]);
                faceliquifyManager:ClearShaderLens(node:GetRoot(),state["index"]+liquifyNum);
              end
              EditorSystem:DestroyNodeTree(node);
              collectgarbage();
            end)
        end
        --if imguifunction.ImGui.MenuItem("Copy", "F11", false, true) then --删除(如何删除?)
        --  local cloned = node:Clone()
        --  node:GetRoot():AttachNode(cloned)
        --end
      end
      if imguifunction.ImGui.MenuItem("Clone","F3",false,true) then  --重命名
        action = defined.ContextAction.Clone;
      end 
      imguifunction.ImGui.EndPopup();
    end
    imguifunction.ImGui.PopStyleVar()
  end
  return action;
end

function sceneview:NewNodeContextMenu()
  local ret = imguifunction.ImGui.BeginPopupContextWindow("New Object", 0, true)
  return ret
end

function sceneview:_ProcessDragDropSource(node)
  --根节点venus root不能拖动
  if node and node:GetTypeName() ~= "Actor" and ImGui.BeginDragDropSource(ImGui.ImGuiDragDropFlags_SourceAllowNullID) then
    local nodeName = node:GetName();
    ImGui.TextUnformatted(nodeName);  --拖拽时显示node名称
    ImGui.SetDragDropPayload("Node", (node:GetObjectID())); --将ID传递过去，之后再通过ID查找NODE
    ImGui.EndDragDropSource();
    return true;
  end
  return false;
end

function sceneview:CheckDrag(node)
  if self:_ProcessDragDropSource(node) == false then
    self:_ProcessDragDropTarget(node);
  end
end

function sceneview:_IsNodeVisible(node)
  --逻辑是：除去EDITOR_SCENE_LAYER/EDITOR_GAME_LAYER/EDITOR_UI_LAYER之外还有其他Layer则需要在场景树显示
  if not venuscore.isNil(node) and node:isTypeOrDriverType(apolloengine.Node:RTTI())
          and not node:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_SCENE_LAYER)
          and not node:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_UI_LAYER) then
    return true
  end
  return false
end

--处理Node UI
function sceneview:DrawNode(node)
  --node 为空
  if node == nil then
    ERROR("node is nil");
    return;
  end
    
  ImGui.PushID((node:GetObjectID()));
  ImGui.AlignTextToFramePadding();
  local is_selected = false;  --标识选中状态
  local selected = EditorSystem:GetSelected();
  if type(selected) == "userdata" and not venuscore.isNil(selected) and selected:GetObjectID() == node:GetObjectID() then
    is_selected = true;
  end
  
  local nodeName = node:GetName();

  

  local nodeFlags = defined.TreeNodeFlag;
  
  if is_selected then
    nodeFlags = nodeFlags + ImGui.ImGuiTreeNodeFlags_Selected;
  end
  

  local no_children = true;
  local childList = {};
  local childCnt = 0;
  childList = node:GetNoClothDebugChildrens();
  childCnt = #childList;
  if childCnt > 0 then  --没有子节点
    local childs = {};
    for k,v in pairs(childList) do
      if self:_IsNodeVisible(v) then
        if v.EditorUIType~=apollocore.IContent.UNVISUAL or EditorSystem:ShowEverything() then
          table.insert(childs, v);
        end
        no_children = false
      end
    end
    table.sort(childs, function(a, b)
      return a:GetName() < b:GetName();
    end);
    childList = childs;
  end
  childCnt = #childList;

  if no_children then --没有子Node,则创建叶子节点
    nodeFlags = nodeFlags + ImGui.ImGuiTreeNodeFlags_Leaf;
  end

  local subSelected = false
  if type(selected) == "userdata" and not venuscore.isNil(selected) then
    while (selected ~= nil and selected:isTypeOrDriverType(apolloengine.Node:RTTI()))
    do
      if selected:GetObjectID() == node:GetObjectID() then
        subSelected = true
        break
      end
      selected = selected:GetRoot()
    end
  end
  
  local pos = ImGui.GetCursorScreenPos();
  ImGui.AlignTextToFramePadding();
  
  local isactive = node:isActiveHierarchy();
  local size = mathfunction.vector2(20, 20)
  local color;
  --当前选中节点是否属于prefab实例
  local isPrefabRoot = node.PrefabPath ~= "";
  local isPrefab = node.HostPrefabPath ~= "";
  if isPrefabRoot then
    color = mathfunction.Color(1,1,0,1);
  elseif isPrefab then
    color = mathfunction.Color(0.7,0.7,0,1);
  else
    color = mathfunction.Color(1,1,1,1);
  end
  color = isactive and color or mathfunction.Color(0.5,0.5,0.5,1)
  ImGui.PushStyleColor(ImGui.ImGuiCol_Text, color);
  ImGui.PushStyleVar(ImGui.ImGuiStyleVar_ItemSpacing, mathfunction.vector2(16, 0))
  local currActive = node.Active
  local opened, checked = ImGui.TreeNodeEx(nodeName, nodeFlags, getNodeIcon(node), size, true, currActive, subSelected);
  if currActive ~= checked then
    local StaticID = node:GetContentPath();
    local cmd = PropertyChangeCmd(StaticID, "Active", checked);
    CommandManager:DoIt(cmd);
  end

  ImGui.PopStyleVar(1)
  ImGui.PopStyleColor(1);
  --处理拖拽的逻辑，先不做
  if self.edit_lable == false then
    self:CheckDrag(node);  --处理拖拽
  end

  --处理编辑Node名称的逻辑
  if self.edit_lable == true and is_selected then
    ImGui.SetCursorScreenPos(pos);
    local contentRegionAvail = ImGui.GetContentRegionAvail();
    local contentRegionAvailWidth = contentRegionAvail.x;
    ImGui.PushItemWidth(contentRegionAvailWidth);
    
    ImGui.PushID((node:GetObjectID()));
    
    local inputTextFlags = ImGui.ImGuiInputTextFlags_EnterReturnsTrue + ImGui.ImGuiInputTextFlags_AutoSelectAll;
    ImGui.PushStyleColor(ImGui.ImGuiCol_Text, mathfunction.Color(1,1,1,1));
    ImGui.PushStyleColor(ImGui.ImGuiCol_FrameBg, mathfunction.Color(0.246,0.246,0.246,1.0));
    local bR, strR = ImGui.InputText("",nodeName,64,inputTextFlags);
    ImGui.PopStyleColor(2);
    if bR == true then
      self.legalName = EditorCheckFunc:CheckLegalName(node, strR);
      if self.legalName then
        if filelfs:CheckChinese(strR) then
          self.chineseChecked = true;
          strR = nodeName;
        end
        node:SetName(strR);
      end
      self.edit_lable = false;
    end
    ImGui.PopItemWidth();
    
    --鼠标点击了别处，取消编辑状态
    if ImGui.IsItemActive() == false and (ImGui.IsMouseClicked(0) or ImGui.IsMouseDragging()) then
      self.legalName = EditorCheckFunc:CheckLegalName(node, strR);
      if self.legalName then
        if filelfs:CheckChinese(strR) then
          self.chineseChecked = true;
          strR = nodeName;
        end
        node:SetName(strR);
      end
      self.edit_lable = false;
    end
    
    ImGui.PopID();
  end
  
  if ImGui.IsItemHovered() and ImGui.IsMouseDragging() == false then
    if ImGui.IsMouseClicked(0) then
      self.currentID = ImGui.GetID((node:GetObjectID()));
    end
    
    local localID = ImGui.GetID((node:GetObjectID()));
    if ImGui.IsMouseReleased(0) and self.currentID == localID then
      if is_selected == false then
        self.edit_lable = false;
      end
      EditorSystem:Select(node); --选中此node
    end

    if ImGui.IsMouseDoubleClicked(0) then
      --LOG("Double Clicked")
      local isScene2D = EditorSystem:isSceneShow2D();
      local editorCamera = isScene2D and EditorSystem:GetEditorCamera2D() or EditorSystem:GetEditorCamera();
      local cameraComponent = editorCamera:GetComponent(apolloengine.Node.CT_CAMERA);

      local viewNode = node;
      if isScene2D then
        viewNode = EditorSystem:Get2DBaseTex();--2D模式用底图作为瞄准对象
        local selectRender = node:GetComponent(apolloengine.Node.CT_RENDER);
        if selectRender and selectRender:GetResourceType() == -200 then --如果是ScreenImage fov放大一点
          cameraComponent.Fov_Size = 2.0;
        else
          cameraComponent.Fov_Size = 0.2;
        end
      end

      local tm = getmetatable(viewNode);
      if tm.GetComponent then
        local node_trans = viewNode:GetComponent(apolloengine.Node.CT_TRANSFORM);
        local distance = 1;
        local fov = cameraComponent:GetFov_Size();
        local bindbox, bDefault = HierarchyTool:GetBindBox(viewNode);
        local focusCenter;

        if bDefault == false then
          local bindbox_max = bindbox:GetMax();
          local bindbox_min = bindbox:GetMin();
          local maxlength = math.max(math.abs(bindbox_max.x-bindbox_min.x),
                  math.abs(bindbox_max.y-bindbox_min.y),
                  math.abs(bindbox_max.z-bindbox_min.z)
          );
          distance = (maxlength/2) / math.tan(fov/2) * 5;
          if distance == 0 then
            distance = 1;
          end
          focusCenter = bindbox:GetCenter();
        else
          focusCenter =  node_trans:GetWorldPosition();
        end

        cameraComponent:LookAt(focusCenter - cameraComponent:GetForward() * distance, focusCenter, mathfunction.vector3(0,1,0));
      end
    end
  end
  
  if self.edit_lable == false then
    local action = self:_CheckContexMenu(node);  --右键菜单
    if action == defined.ContextAction.Rename then
      self.edit_lable = true;
      EditorSystem:Select(node);
    elseif action == defined.ContextAction.Clone then
      local cloneCmd = CloneNodeCommand(node:GetStaticID());
      CommandManager:DoIt(cloneCmd);
    end
  end

  if opened == true then
    if no_children == false then  --迭代子节点
      for i = 1, childCnt do
        self:DrawNode(childList[i]);
      end
    end
    ImGui.TreePop();
  end

  ImGui.PopID();
end

function sceneview:Render(apolloRootNode)
  
  --draw editor camera
  --local editorCamera = EditorSystem:GetEditorCamera();
  --self:DrawNode(editorCamera:GetNativeNode());
  
  if self.bodyfail==true then
    local windowFlags = bit.bor(ImGui.ImGuiWindowFlags_NoScrollbar,ImGui.ImGuiWindowFlags_NoResize,ImGui.ImGuiWindowFlags_NoSavedSettings,ImGui.ImGuiWindowFlags_NoDocking);
    local b1,show = ImGui.Begin("Body Fail", true, windowFlags);
    ImGui.Text("scene not select main camera!");
    if ImGui.Button("Ok ") then
      self.bodyfail = false;
    end
    ImGui.End();
  end
  
  self:_CheckContexMenu();
  self:DrawNode(apolloRootNode);
  self:ShowDeletePrompt();
end

--设置hierachy要遍历的Scene
function sceneview:SetScene(scene)
  self._scene = scene;
end

function sceneview:OnGui()
  if self._scene == nil or venuscore.isNil(self._scene) then
    self._scene = EditorSystem:GetEditScene();
  end
  
  local apolloRootNode = self._scene:GetRootNode();

  ImGui.BeginChild("background");--处理拖拽到空白处的逻辑
  self.hasdropdown = false;
  local flag = defined.TreeNodeFlag + ImGui.ImGuiTreeNodeFlags_DefaultOpen
  ImGui.PushStyleVar(ImGui.ImGuiStyleVar_IndentSpacing, 16)
  ImGui.AlignTextToFramePadding()
  local size = mathfunction.vector2(20, 20)
  local selected, _ = ImGui.TreeNodeEx(self._scene:GetName(), flag, getNodeIcon(apolloRootNode), size, false, false, false)
  if selected then
    if ImGui.IsItemClicked() then
      EditorSystem:Select(self._scene);
    end
    self:Render(apolloRootNode);
    ImGui.TreePop();
  end
  ImGui.PopStyleVar()
  ImGui.EndChild();
  if not self.hasdropdown then
    self:_ProcessDragDropTarget(apolloRootNode);
  end
  if not self.legalName then
    local windowFlags = bit.bor(ImGui.ImGuiWindowFlags_NoScrollbar,ImGui.ImGuiWindowFlags_AlwaysAutoResize,
            ImGui.ImGuiWindowFlags_NoSavedSettings,ImGui.ImGuiWindowFlags_NoDocking, ImGui.ImGuiWindowFlags_NoTitleBar);
    ImGui.OpenPopup("Name Repeat");
    if ImGui.BeginPopupModal("Name Repeat", true, windowFlags) then
      ImGui.TextColored(mathfunction.vector4(1,0,0,1.0),"The name already exists in same level,please re-enter");
      local contentRegionAvail = ImGui.GetContentRegionAvail();
      local contentRegionAvailWidth = contentRegionAvail.x;
      local contentRegionAvailHeight = contentRegionAvail.y;
      local currentCursorPos = ImGui.GetCursorPos();
      ImGui.SetCursorPos(mathfunction.vector2(currentCursorPos.x + contentRegionAvailWidth /2.5,currentCursorPos.y + contentRegionAvailHeight / 2));
      if ImGui.Button("Ok ") then
        self.legalName = true;
        ImGui.CloseCurrentPopup();
      end
      ImGui.EndPopup();
    end
  end
  if self.chineseChecked then
    panelutility.ErrorView("Name Error", "Please do not use chinese", function () self.chineseChecked = false; end);
  end
  if self.dragIllegal then
    panelutility.ErrorView("drag Illegal", self.dragIllegal, function () self.dragIllegal = nil; end);
  end
end

return sceneview;
