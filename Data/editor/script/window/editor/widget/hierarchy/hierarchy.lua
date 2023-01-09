local EditorSystem = require "window.editor.system.editsystem"
local imguifunction = require "imguifunction"
local Math = require "Math"
local Engine = require "Engine"
local MeteorCore = require "MeteorCore"
local Object = require("MeteorCore.core.Object")
local sceneview = require "window.editor.widget.hierarchy.sceneview"
local hierarchyresource = require "window.editor.widget.hierarchy.hierarchyresource"

local hierarchy = class("hierarchy", Object)

local ImGui = imguifunction.ImGui


function hierarchy:ctor()
  self.edit_lable = false;
  self.currentID = 0; --stand for empty
  self.titleName = "Hierarchy";
  self.isOnGui = true;
  self.hasdropdown = false;
  self.chineseChecked = false;
  self.mode = "scene";
  self.isShow2D = false;
  self.sceneview = sceneview.new();
  self.showPrefabDialog = false;
  loadResources()
end

--这里删掉的有点多 一会看看有没问题

--设置hierachy要遍历的Scene
--show preview scene的时候会改成facecutescene
function hierarchy:SetScene(scene)
  self._scene = scene;
  self.sceneview:SetScene(scene);
end

function hierarchy:ShowPrefabEditorDialog()
  if not self.showPrefabDialog then
    return;
  end
end

function hierarchy:OnGui()
  if self.isOnGui then
    ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowPadding, imguifunction.ImVec2(0, 8))
    local dockFlag = ImGui.ImGuiWindowFlags_NoScrollbar + ImGui.ImGuiWindowFlags_NoCollapse
    local b
    b, self.isOnGui = ImGui.Begin(self.titleName, true, dockFlag);
    ImGui.BeginChild("background");--处理拖拽到空白处的逻辑
    ImGui.InvisibleButton("#invisivle", imguifunction.ImVec2(0.1, 4))
    ImGui.SameLine()
    local size = imguifunction.ImVec2(20, 20)
    local clicked = ImGui.ImageButtonEx(hierarchyresource.newIcon.icon, size, hierarchyresource.newIcon.tip, false, true)
    if clicked or self.shownContextMenu then
      imguifunction.ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowPadding, imguifunction.ImVec2(8, 8))
      self.shownContextMenu = HierarchyTool:NewNodeContextMenu();
      imguifunction.ImGui.PopStyleVar()
    end

    if EditorSystem:GetPrefabMode() then
      if ImGui.Button("<  Exit Prefab Editor", imguifunction.ImVec2(ImGui.GetWindowWidth(),0.0)) then
        self.showPrefabDialog = true;
      end
      PrefabView:OnGui();
    else
      --上传素材
      --上传素材要先分档打包素材
      ImGui.SameLine();
      if ImGui.Button("Upload") then
        GlobalUploadWindow:Open();
      end
      ImGui.SameLine()
      clicked = ImGui.ImageButtonEx(hierarchyresource.layerIcon.icon, size, hierarchyresource.layerIcon.tip, false, true)
      if clicked or self.shownHierarchyMenu then
        self.shownContextMenu = ImGui.OpenPopup("hierarchy_view");
      end
    end

    ImGui.EndChild();
    ImGui.End();
    ImGui.PopStyleVar()
    self:ShowPrefabEditorDialog();
  end
end

function hierarchy:SetTitleName(titleName)
  self.titleName = titleName;
end

function hierarchy:GetTitleName()
  return self.titleName;
end

function hierarchy:IsOnGui()
  return self.isOnGui;
end

function hierarchy:SetOnGui(isOnGui)
  self.isOnGui = isOnGui;
end

return hierarchy;



