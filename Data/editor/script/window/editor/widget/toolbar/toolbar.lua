
local EditorSystem = require "window.editor.system.editsystem"
local imguifunction = require "imguifunction"
local Engine = require "Engine"
local Math = require "Math"
local Core = require "MeteorCore"
local Object = require "MeteorCore.core.Object"
local defined = require "window.editor.system.defined"
local BundleManager = require "window.editor.system.BundleManager"

local filelfs = require "window.editor.widget.assets.fileutility"


local ImGui = imguifunction.ImGui;

local toolbar = class("toolbar", Object)

function toolbar:ctor()
  self.titleName = "Toolbar";
  self.isOnGui = true;
end

function toolbar:DrawToolbar()
  local icons = EditorSystem:GetIcons();
  local operationType = EditorSystem:GetOperationType();
  local showGrid = EditorSystem:GetShowGrid();
  local contentRegionAvail = imguifunction.ImGui.GetContentRegionAvail();
  local contentRegionAvailWidth = contentRegionAvail.x;
  
  if ImGui.ToolbarButtonEx(icons["Translate"],"TRANSLATE", operationType == ImGui.ImTranslate, true) then
    EditorSystem:SetOperationType(ImGui.ImTranslate);
  end
  ImGui.SameLine();
  if ImGui.ToolbarButtonEx(icons["Rotation"],"ROTATE", operationType == ImGui.ImRotate, true) then
    EditorSystem:SetOperationType(ImGui.ImRotate);
  end
  ImGui.SameLine();
  if ImGui.ToolbarButtonEx(icons["Scale"],"SCALE", operationType == ImGui.ImScale, true) then
    EditorSystem:SetOperationType(ImGui.ImScale);
  end
	ImGui.SameLine();

  if ImGui.ToolbarButtonEx(icons["Grid"],"SHOW GRID", showGrid,true) then
    local isShowGrid = (showGrid == false and true);
    EditorSystem:SetShowGrid(isShowGrid);
  end
  ImGui.SameLine(contentRegionAvailWidth / 2.0 - 36.0,-1.0);
  
  if ImGui.ToolbarButtonEx(icons["Play"],"PLAY", EditorSystem:IsPlay(),true) then
    if EditorSystem:IsPlay() then
      EditorSystem:Stop();
    else
      self:_SaveTempScene();
      EditorSystem:Play();
    end
    
  end
  ImGui.SameLine();
  if ImGui.ToolbarButtonEx(icons["Pause"],"PAUSE",EditorSystem:IsPause(),true) then
    if EditorSystem:IsPause() then
      EditorSystem:Play();
    else
      EditorSystem:Pause();
    end
  end
  ImGui.SameLine();
  if ImGui.ToolbarButtonEx(icons["Stop"], "STOP", false,true) then
    EditorSystem:Stop();
  end
  ImGui.SameLine();
end


function toolbar:OnGui()
  if self.isOnGui then
    local b;
    local dockFlag = ImGui.ImGuiWindowFlags_NoScrollbar + ImGui.ImGuiWindowFlags_NoCollapse
    b,self.isOnGui = ImGui.Begin(self.titleName, true, dockFlag);
    self:DrawToolbar();
    imguifunction.ImGui.End();
  end
end

function toolbar:GetTitleName()
  return self.titleName;
end

function toolbar:IsOnGui()
  return self.isOnGui;
end

function toolbar:SetOnGui(isOnGui)
  self.isOnGui = isOnGui;
end

function toolbar:_SaveTempScene()
    --运行用户较本前，先临时保存场景
  local tempScenePath = SystemDirectories["temp"];
  local relative_bundlepath = tempScenePath .. "autosavescene.tmp";
  local bundlepath = venuscore.IFileSystem:PathAssembly(relative_bundlepath);
  BundleManager:SaveScene(bundlepath);
end



return toolbar;