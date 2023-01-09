local windowindex = require "window.windowindex"
local imguifunction = require "imguifunction"
local Engine = require "Engine"
local Math = require "Math"
local Core = require "MeteorCore"
local Object = require "MeteorCore.core.Object"
local filelfs = require "window.editor.widget.assets.fileutility"


local ImGui = imguifunction.ImGui;

local selectscene = class("selectscene", Object)

function selectscene:ctor()
  self.open = false;
  self.title = "Select Scene";
end

function selectscene:Open(caller, selectCallBack)
  if not self.open then
    self.scenes = {};
    local projPath = Core.IFileSystem:Instance():PathAssembly("proj:");
    local scenes = filelfs:SearchFiles(projPath, {"scene"});
    local n = #projPath + 1;
    for i = 1, #scenes do
      local scenePath = string.sub(scenes[i], n);
      table.insert(self.scenes, scenePath);
    end
    self.open = true;
    self.caller = caller;
    self.selectCallBack = selectCallBack;
    ImGui.OpenPopup(self.title);
  end
end

function selectscene:Draw()
  ImGui.SetNextWindowSize(imguifunction.ImVec2(600, 500));
  if ImGui.BeginPopupModal(self.title, true, ImGui.ImGuiWindowFlags_NoResize) then
    if self.open then
      for i = 1, #self.scenes do
        if ImGui.Selectable(self.scenes[i]) then
          local sceneFile = self.scenes[i];
          self.selectCallBack(self.caller, sceneFile); --打开场景
          self:Close();
        end
      end
    end
    ImGui.EndPopup();
  elseif self.open then
    self.selectCallBack(self.caller, "");
    self:Close();
  end
end

function selectscene:Close()
  self.open = false;
  self.caller = nil;
  self.selectCallBack = nil;
  ImGui.CloseCurrentPopup();
end

return selectscene;