local EditorSystem = require "window.editor.system.editsystem"
local imguifunction = require "imguifunction"
local Engine = require "Engine"
local Math = require "Math"
local Core = require "MeteorCore"
local defined = require "window.editor.system.defined"
local ImGui = imguifunction.ImGui;
local filelfs = require "window.editor.widget.assets.fileutility"
local bit = require("bit")
local Object = require "MeteorCore.core.Object"


local ObjectSelector = class("ObjectSelector", Object)



function ObjectSelector:ctor()
  self.TitleName = "Object Selector";
  self._CurrentSelectType = "Any"; --默认选择任意
  self._CurrentSelectIndex = 1;
  self._CurrentSelectScript = "Any";  --过滤任意脚本对象
  self._CurrentSelectScriptIndex = 1;
  self._IsOpen = false;
  self._TypeFilter = Engine.GObject:RTTI();  --默认过滤NODE
  self._InternalFilter = nil;
  self._ScriptFilter = nil;  --脚本对象过滤
  self._SelectedObject = nil;
  self._CurrentID = nil;
  self._SelectedCallback = nil;
  self._FilterCallback = nil; -- 通过回调函数自定义过滤 回调函数只传入当前的obj(type_filter过滤后的)
  self._TopNode = nil; -- 不是从scenenode开始，而是由指定的node可以选择
end

function ObjectSelector:Open(type_filter,selected_callback, filter_callback, topNode)
  self._IsOpen = true;
  self._TypeFilter = type_filter;
  self._SelectedCallback = selected_callback;
  self._FilterCallback = filter_callback;
  self._TopNode = topNode
end



function ObjectSelector:Render(apolloRootNode)
  self:DrawNode(apolloRootNode);
end

function ObjectSelector:FilterObject(obj)
  local res = false;
  if obj then
    if obj:isType(self._TypeFilter) or obj:isTypeOrDriverType(self._TypeFilter) then
      res = true;
      if self._InternalFilter then
        --Node需要在判断是否挂了某一种类型的Cmponent
        if obj:isTypeOrDriverType(Engine.Node:RTTI()) then
          if self._InternalFilter then
            if obj:HaveComponent(self._InternalFilter:GetTypeName()) == false then  --是否有指定的Component
              res = false;
            else
              if self._ScriptFilter then
                local scriptCom = obj:GetComponent(Engine.Node.CT_SCRIPT);
                local instanceList = scriptCom.Instances;
                for key, value in pairs(instanceList) do
                  if value:isType(self._ScriptFilter) then
                    res = true;
                  end
                end
              end
            end
          end
        --如果指定的类型是IComponent还可以继续指定具体component类型
        elseif obj:isTypeOrDriverType(Engine.Component:RTTI()) then  
          if self._InternalFilter then
            if obj:isType(self._InternalFilter) == false then  --是否是指定类型Component
              res = false;
            else
              if self._ScriptFilter then
                local instanceList = obj.Instances;
                for key, value in pairs(instanceList) do
                  if value:isType(self._ScriptFilter) then
                    res = true;
                  end
                end
              end
            end
          end 
        end
      end

      if res and self._FilterCallback ~= nil then
        res = self._FilterCallback(obj);
      end

    end
  end
  return res;
end

function ObjectSelector:FilterUI()
  --只有当外部提供的筛选是Node或其派生类，
  --或者IComponent时才提供额外筛选;如果外部确定了Component类型则不再提供额外筛选
  if (self._TypeFilter.isType and self._TypeFilter:isType(apolloengine.IComponent:RTTI())) or
    (self._TypeFilter.isTypeOrDriverType and self._TypeFilter:isTypeOrDriverType(apolloengine.Node:RTTI())) then
    --还可以过滤Component
    local iComponent = apolloengine.IComponent:GetTypeName();
    local allComponentList = libvenuscore.RttiManager:GetChildrenName(iComponent);
    local allComponentRTTIList = libvenuscore.RttiManager:GetChildrenRtti(iComponent);
    table.insert(allComponentList,1,"Any");
    
    if self._TypeFilter:isTypeOrDriverType(apolloengine.Node:RTTI()) then
      ImGui.Text("With Component:    ");
    else
      ImGui.Text("Filter:   ");
    end
    ImGui.SameLine();
    local isSelectType = ImGui.BeginCombo("##Component",self._CurrentSelectType);
    if isSelectType then
      for i = 1,#allComponentList do
        local isSelected = self._CurrentSelectType == allComponentList[i];
        if ImGui.Selectable(allComponentList[i]) then
          self._CurrentSelectType = allComponentList[i];
          self._CurrentSelectIndex = i;
        end
      end
      ImGui.EndCombo();
    end
    
    self._InternalFilter = self._CurrentSelectIndex ~= 1 and allComponentRTTIList[self._CurrentSelectIndex - 1] or nil;
    
    --如果选择的是ScriptComponent,就再加一个删选器,筛选具体的脚本对象
    if allComponentList[self._CurrentSelectIndex] == apolloengine.ScriptComponent:GetTypeName() then
      ImGui.NewLine();
      ImGui.Text("Scripts:   ");
      ImGui.SameLine();
      local scripNametList,scriptInsList = rttimanager:GetAllBehavior();
      table.insert(scripNametList,1,"Any");
      local isSelectType = ImGui.BeginCombo("##Script",self._CurrentSelectScript);
      if isSelectType then
        for i = 1,#scripNametList do
          local isSelected = self._CurrentSelectScript == scripNametList[i];
          if ImGui.Selectable(scripNametList[i]) then
            self._CurrentSelectScript = scripNametList[i];
            self._CurrentSelectScriptIndex = i;
          end
        end
        ImGui.EndCombo();
      end
      self._ScriptFilter = self._CurrentSelectScriptIndex ~= 1 and 
        scriptInsList[self._CurrentSelectScriptIndex - 1] or nil;
    else
      self._ScriptFilter = nil;
    end
    ImGui.Separator();
  end
end

--叶子节点并且不符合过滤条件的Node不予显示
function ObjectSelector:FilterLeafNode(node)
  local res = false;
  local showChildNode = 0;
  if node:isTypeOrDriverType(Engine.Component:RTTI()) == false then
    local childList = node:GetChildrens();
    local childCnt = #childList;
    for i = 1, childCnt do
      local apolloChildNode = childList[i];
      local editorCamera = EditorSystem:GetEditorCamera();
      --逻辑是：除去EDITOR_SCENE_LAYER/EDITOR_GAME_LAYER/EDITOR_UI_LAYER之外还有其他Layer则需要在场景树显示
      if apolloChildNode:isTypeOrDriverType(apolloengine.Node:RTTI()) 
        and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_SCENE_LAYER)
        and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_UI_LAYER)
        and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_CLOTHDEBUG_LAYER) then
          showChildNode = showChildNode + 1;
      end
    end
    if showChildNode == 0 then
      local fr = self:FilterObject(node);
      if fr == false then
        local components = node.Components;
        for key, value in pairs(components) do
          fr = fr or self:FilterObject(value);
        end
      end
      res = not fr;
    end
  end
  return res;
end

--判断是否是叶子节点
function ObjectSelector:IsLeafNode(node)
  local res = false;
  local childList = node:GetChildrens();
  local childCnt = #childList;
  local validChild = 0;
  for i = 1, childCnt do
    local apolloChildNode = childList[i];
    --逻辑是：除去EDITOR_SCENE_LAYER/EDITOR_GAME_LAYER/EDITOR_UI_LAYER之外还有其他Layer则需要在场景树显示
    if apolloChildNode:isTypeOrDriverType(apolloengine.Node:RTTI()) 
      and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_SCENE_LAYER)
      and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_UI_LAYER) then
        validChild = validChild + 1;
    end
  end
  if validChild == 0 then
    res = true;
  end
  return res;
end

function ObjectSelector:IsValidNode(node)
  local res = false;
  if node.isTypeOrDriverType and node:isTypeOrDriverType(apolloengine.Node:RTTI()) 
      and not node:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_SCENE_LAYER)
      and not node:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_UI_LAYER) then
        res = true;
  end
  return res;
end


function ObjectSelector:ScriptTree(node)
  local nodeFlags = imguifunction.ImGui.ImGuiTreeNodeFlags_AllowItemOverlap + 
    imguifunction.ImGui.ImGuiTreeNodeFlags_DefaultOpen;
  local nodeName = "";
  if type(node) == "table" and node:isType(self._TypeFilter) then
    nodeName = node:GetTypeName();
    nodeFlags = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_Leaf;
  else
    local isValidNode = self:IsValidNode(node);
    if isValidNode then
      nodeName = node:GetName();
      nodeFlags = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_OpenOnArrow;
    else
      return;
    end
  end
  
  imguifunction.ImGui.PushID((node:GetObjectID()));
  imguifunction.ImGui.AlignTextToFramePadding();
  local pos = imguifunction.ImGui.GetCursorScreenPos();
  imguifunction.ImGui.AlignTextToFramePadding();
  local opened = imguifunction.ImGui.TreeNodeEx(nodeName,nodeFlags);
  if imguifunction.ImGui.IsItemHovered() and imguifunction.ImGui.IsMouseDragging() == false then
    if imguifunction.ImGui.IsMouseClicked(0) then
      self._CurrentID = imguifunction.ImGui.GetID((node:GetObjectID()));
    end
    local localID = imguifunction.ImGui.GetID((node:GetObjectID()));
    if imguifunction.ImGui.IsMouseReleased(0) and self.currentID == localID then
      self._SelectedObject = node;
    end

    if imguifunction.ImGui.IsMouseDoubleClicked(0) then
      if self._SelectedCallback and 
        type(node) == "table" and 
        node:isType(self._TypeFilter) then
        self._SelectedCallback(node);
        self:Close();
      end
    end
  end
  --Script Component
  if node.isTypeOrDriverType and node:isTypeOrDriverType(apolloengine.Node:RTTI()) then
    local scripComp = node:GetComponent(apolloengine.Node.CT_SCRIPT);
    if scripComp then
      local instList = scripComp.Instances;
      for key, value in pairs(instList) do
        if value:isType(self._TypeFilter) then
          self:ScriptTree(value);
        end
      end
    end
  end
     
  if opened == true then
    if node.isTypeOrDriverType and node:isTypeOrDriverType(apolloengine.Node:RTTI()) then
      local childList = node:GetChildrens();
      local childCnt = #childList;
      for i = 1, childCnt do
        local apolloChildNode = childList[i];
        self:ScriptTree(apolloChildNode);
      end
    end
    imguifunction.ImGui.TreePop();
  end
  imguifunction.ImGui.PopID();
end


--处理Node UI
function ObjectSelector:ObjectTree(node)
  --为空或者不符合过滤条件
  if node == nil or self:FilterLeafNode(node) then
    return;
  end
  
  local nodeName = "";
  local no_children = true;
  local childList = nil;
  local childCnt = 0;
  if node:isTypeOrDriverType(apolloengine.Node:RTTI()) or node:GetTypeName() == "Actor" then
    nodeName = node:GetName();
    childList = node:GetChildrens();
    childCnt = #childList;
  elseif node:isTypeOrDriverType(apolloengine.IComponent:RTTI()) then
    nodeName = node:GetTypeName();
  else
    return;
  end

  imguifunction.ImGui.PushID((node:GetObjectID()));
  imguifunction.ImGui.AlignTextToFramePadding();
  local is_selected = false;  --标识选中状态
  


  local nodeFlags = imguifunction.ImGui.ImGuiTreeNodeFlags_AllowItemOverlap + 
    imguifunction.ImGui.ImGuiTreeNodeFlags_DefaultOpen;


  if childCnt > 0 then  --有子节点
    no_children = false;
    nodeFlags = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_OpenOnArrow;
  end
  
  if no_children == true then --没有子Node,则创建叶子节点
    nodeFlags = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_Leaf;  
  end
  
  local pos = imguifunction.ImGui.GetCursorScreenPos();
  imguifunction.ImGui.AlignTextToFramePadding();
  
  local opened = imguifunction.ImGui.TreeNodeEx(nodeName,nodeFlags);
  
  if imguifunction.ImGui.IsItemHovered() and imguifunction.ImGui.IsMouseDragging() == false then
    if imguifunction.ImGui.IsMouseClicked(0) then
      self._CurrentID = imguifunction.ImGui.GetID((node:GetObjectID()));
    end
    
    local localID = imguifunction.ImGui.GetID((node:GetObjectID()));
    if imguifunction.ImGui.IsMouseReleased(0) and self.currentID == localID then
      self._SelectedObject = node;
    end

    if imguifunction.ImGui.IsMouseDoubleClicked(0) then
      if self._SelectedCallback and self:FilterObject(node) then
        self._SelectedCallback(node);
        self:Close();
      end
    end
  end
  
  --components
  if node:isTypeOrDriverType(apolloengine.Node:RTTI()) 
    and self._TypeFilter:isTypeOrDriverType(apolloengine.IComponent:RTTI()) 
    or self._TypeFilter:isTypeOrDriverType(apolloengine.IContent:RTTI()) then
    local components = node.Components;
    if components then
      for key, value in pairs(components) do
        if self:FilterObject(value) then
          self:ObjectTree(value);
        end
      end
    end
  end
  
  
  if opened == true then
    if no_children == false then  --迭代子节点
      for i = 1, childCnt do
        local apolloChildNode = childList[i];
        local editorCamera = EditorSystem:GetEditorCamera();
        --逻辑是：除去EDITOR_SCENE_LAYER/EDITOR_GAME_LAYER/EDITOR_UI_LAYER之外还有其他Layer则需要在场景树显示
        if apolloChildNode:isTypeOrDriverType(apolloengine.Node:RTTI()) and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_SCENE_LAYER)
          and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_UI_LAYER) 
          and not apolloChildNode:isLayer(apolloengine.LayerMask.MC_MASK_EDITOR_CLOTHDEBUG_LAYER) then
          self:ObjectTree(apolloChildNode);
        end
      end
    end
    imguifunction.ImGui.TreePop();
  end
  imguifunction.ImGui.PopID();
end

function ObjectSelector:Close()
  self._CurrentSelectType = "Any"; --默认选择任意
  self._CurrentSelectIndex = 1;
  self._CurrentSelectScript = "Any";  --过滤任意脚本对象
  self._CurrentSelectScriptIndex = 1;
  self._IsOpen = false;
  self._TypeFilter = apolloengine.Node:RTTI();  --默认过滤NODE
  self._InternalFilter = nil;
  self._SelectedObject = nil;
  self._CurrentID = nil;
  self._SelectedCallback = nil;
  self._FilterCallback = nil;
  self._TopNode = nil;
end


function ObjectSelector:OnGui()
  if self._IsOpen then
    --local apolloRootNode = Engine.SceneManager:GetRootNode("Scene");
    local apolloRootNode = nil;

    if EditorSystem:GetPrefabMode() then
      apolloRootNode = EditorSystem.currentEditPrefabNode;
    else
      local editScene = EditorSystem:GetEditScene();
      apolloRootNode = editScene:GetRootNode();
    end

    if self._TopNode then -- override scene's RootNode
      apolloRootNode = self._TopNode
    end

    local b1,close = imguifunction.ImGui.Begin(self.TitleName, true, imguifunction.ImGui.ImGuiWindowFlags_NoScrollbar);
    if close == false then
      self:Close();
    end
    
    --过滤框
    self:FilterUI();
    
    if type(self._TypeFilter) == "table" and self._TypeFilter:isType(VenusBehavior) then
      self:ScriptTree(apolloRootNode);
    else
      self:ObjectTree(apolloRootNode);
    end
    
    

    imguifunction.ImGui.End();
  end
end


return ObjectSelector;



