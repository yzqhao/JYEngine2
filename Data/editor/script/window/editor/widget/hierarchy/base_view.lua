local EditorSystem = require "window.editor.system.editsystem"
local imguifunction = require "imguifunction"
local Math = require "Math"
local Engine = require "Engine"
local MeteorCore = require "MeteorCore"
local Object = require("MeteorCore.core.Object")
local defined = require "window.editor.system.defined"
local filelfs = require "window.editor.widget.assets.fileutility"
local bit = require("bit")

local ImGui = imguifunction.ImGui

local baseview = class("baseview", Object)
--prefab_view不继承这个

function baseview:ctor()
  self.edit_lable = false;
  self.currentID = 0; --stand for empty
  self.hasdropdown = false;
  self.chineseChecked = false;

  self.ShowDeletePromptDialog = false;
  self.DeleteCallback = nil;
  self.DeleteNode = nil;
end

function baseview:DeletePrompt(node,callback)
  self.ShowDeletePromptDialog = true;
  self.DeleteCallback = callback;
  self.DeleteNode = node;
end

function baseview:ShowDeletePrompt()
  if self.ShowDeletePromptDialog == false then
    return;
  end
  
  local node = self.DeleteNode;
end

--右键菜单逻辑
function baseview:_CheckContexMenu(node)
  local action = defined.ContextAction.None;
  if node ~= nil then --在Node上触发鼠标右键
    ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowPadding, imguifunction.ImVec2(8, 8))
    if ImGui.BeginPopupContextItem("Entity Context Menu",1) then
      EditorSystem:Select(node);
      if self:IsAllowRename(node) and ImGui.MenuItem("Rename","F2",false,true) then  --重命名
        action = defined.ContextAction.Rename;
      end
      --local scene = EditorSystem:GetEditScene();
      if (node._Script or (node:GetObjectID() ~= self._scene:GetRootNode():GetObjectID() and node ~= self._scene)) then
        --layerview/sceneview/renderorderview 删除node只允许普通节点或者prefab实例顶节点删除
        --FIXME(hjh) prefab编辑支持嵌套的时候需要修改判断是否可删除逻辑
        if ImGui.MenuItem("Delete","DEL",false, node.PrefabPath ~= "" or node.HostPrefabPath == "") then --删除(如何删除?)
          self:DeletePrompt(node,
            function()
              if(Drag:GetTheObjectID() == node:GetObjectID()) then --要删除的节点和当前选中的节点是同一个节点
                Drag:DropObject();
                EditorSystem:Select(nil);
              end
              EditorSystem:DestroyNodeTree(node);
              collectgarbage();
            end)
        end
        --if ImGui.MenuItem("Copy", "F11", false, true) then --删除(如何删除?)
        --  local cloned = node:Clone()
        --  node:GetRoot():AttachNode(cloned)
        --end
      end
      if imguifunction.ImGui.MenuItem("Clone","F3",false,true) then  --重命名
        action = defined.ContextAction.Clone;
      end 
      
      ImGui.EndPopup();
    end
    ImGui.PopStyleVar()
  end
  return action;
end

function baseview:IsAllowRename(node)
  if not EditorSystem:GetPrefabMode() then --场景编辑时
    --prefab实例top节点 普通节点二者相等
    if node.PrefabPath ~= node.HostPrefabPath then  --prefab实例非top节点不允许改名字(作为private内容对待)
      return false;
    end
  end
  return true;
end


return baseview;