
require("functions")
local windowindex = require "window.windowindex"
local imguifunction = require "imguifunction"
local Math = require("Math")
local Engine = require("Engine")
local Core = require("MeteorCore")
local Object = require("MeteorCore.core.Object")
local bit = require("bit")
local selectscene = require "window.editor.widget.selectwindow.selectscene"
local inputbox = require "window.editor.widget.selectwindow.inputbox"
local ProjectManager = require "window.project.projectmanager"

local ImGui = imguifunction.ImGui;

local project = class("project", Object)

local dw = 800;
local dh = 600;
function project:ctor()
  self.ProjectManager = ProjectManager.new();
  self.opened = true;
  self.window = nil;
  self.openscene = selectscene.new();
  self.newscene = inputbox.new("Create new scene", "newscene");
end

function project:Show()
end

function project:DrawStartPage()
  local projectPath = self.ProjectManager:GetProjectPath();

  local windowFlags = bit.bor(ImGui.ImGuiWindowFlags_NoTitleBar,ImGui.ImGuiWindowFlags_NoScrollbar,
    ImGui.ImGuiWindowFlags_NoResize,ImGui.ImGuiWindowFlags_NoMove,ImGui.ImGuiWindowFlags_NoSavedSettings);
  
  ImGui.AlignTextToFramePadding();
  if projectPath then
    ImGui.TextUnformatted(projectPath);
  else
    ImGui.TextUnformatted("Please select a project folder.");
  end

  ImGui.SameLine();

  if ImGui.Button("Select Project") then
    Core.IApplication:Instance():GetMainWindow():OpenFloderDialog("");
    self.isOpenDialog = true;
  end
  
  if self.isOpenDialog == true then
    local selectedPath = Core.IApplication:Instance():GetMainWindow():GetSelectedPath();
    if #selectedPath ~= 0 then
      self.ProjectManager:CreateProject(selectedPath[1]);
    end
  end

  if projectPath then
    ImGui.SameLine();

    if ImGui.Button("Select Scene") then
      self.openscene:Open(self.ProjectManager, self.ProjectManager.SelectScene);
    end

    ImGui.SameLine();

    if ImGui.Button("New Scene") then
      self.newscene:Open(self.ProjectManager, self.ProjectManager.CreateScene);
    end

    self.openscene:Draw();
    self.newscene:Draw();
  end
  
  return self.ProjectManager.sceneFile ~= "";
end

function project:Update(timespan)
  local windowData = {};
  windowData["Index"] = windowindex.project;
  windowData["Data"] = {};
  --local newIndex = windowindex.project;

  if self.window then
      local mainSizeW,mainSizeH =  self.window:GetWindowSize();
      ImGui.SetNextWindowSize(imguifunction.ImVec2(mainSizeW, mainSizeW));
      ImGui.SetNextWindowPos(imguifunction.ImVec2(0, 0));
   else
      ImGui.SetNextWindowSize(imguifunction.ImVec2(dw, dh), ImGui.ImGuiCond_FirstUseEver);
   end
  
  _, self.opened = ImGui.Begin("Project", self.opened, ImGui.ImGuiWindowFlags_MenuBar);
  if self.opened == false then
    Core.IApplication:Instance():Abort();
    ImGui.End();
    return windowData;
  end
  
  local result = self:DrawStartPage();
  if result == true then
    --newIndex = windowindex.editor;
    windowData["Index"] = windowindex.editor;
    windowData["Data"]["ProjectManager"] = self.ProjectManager; --将当前的ProjectManager传给下面的window
  end
  ImGui.End();
  return windowData;
end

function project:Close()
end

return project;
