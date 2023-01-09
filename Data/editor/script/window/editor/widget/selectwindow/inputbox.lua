local windowindex = require "window.windowindex"
local imguifunction = require "imguifunction"
local Engine = require "Engine"
local Math = require "Math"
local Object = require "MeteorCore.core.Object"
local ImGui = imguifunction.ImGui;

local inputbox = class("inputbox", Object)

local g_PipelineType = Engine.CameraComponent.DEFERRED_RENDERING;

function inputbox:ctor(title, text)
  self.open = false;
  self.title = title;
  self.text = text or "default";
  self.tip = "";
  self.advancedShading = false;
end

function inputbox:Open(caller, inputCallBack)
  if not self.open then
    self.caller = caller;
    self.inputCallBack = inputCallBack;
    ImGui.OpenPopup(self.title);
    self.open = true;
  end
end

function inputbox:Draw()
  ImGui.SetNextWindowSize(imguifunction.ImVec2(320, 130));
  if ImGui.BeginPopupModal(self.title, true, ImGui.ImGuiWindowFlags_NoResize) then
    if self.open then
      local changed, text = ImGui.InputText("", self.text, 30, ImGui.ImGuiInputTextFlags_AutoSelectAll);
      self.text = text;
      ImGui.SameLine();
      if ImGui.Button("OK") then
        local valid, tip = self.inputCallBack(self.caller, self.text);
        if valid then
          self:_Close();
        else
          self.tip = tip;
        end
      end
      ImGui.SameLine();
      if ImGui.Button("Cancel") then
        self.inputCallBack(self.caller);
        self:_Close();
      end
      if self.tip ~= "" then
        ImGui.TextColored(Math.Vec4(1,0,0,1), self.tip);
      end
    end
    ImGui.EndPopup();
  elseif self.open then
    self.inputCallBack(self.caller);
    self:_Close();
  end
end

function inputbox:_Close()
  self.open = false;
  self.tip = "";
  self.inputCallBack = nil;
  self.caller = nil;
  ImGui.CloseCurrentPopup();
end

return inputbox;