
local editsystem = require "window.editor.system.editsystem"
local imguifunction = require "imguifunction"
local Math = require "Math"
local Engine = require "Engine"
local MeteorCore = require "MeteorCore"
local Object = require("MeteorCore.core.Object")

local ImGui = imguifunction.ImGui;

local inspector = class("inspector", Object)

function inspector:ctor()
  self.titleName = "Inspector";
  self.isOnGui = true;
end


function inspector:OnGui(timespan)  
  if self.isOnGui then
    local b;
    local dockFlag = ImGui.ImGuiWindowFlags_NoScrollbar + ImGui.ImGuiWindowFlags_NoCollapse
    b,self.isOnGui = ImGui.Begin(self.titleName, true, dockFlag);
    ImGui.PushStyleVar(ImGui.ImGuiStyleVar_IndentSpacing, 5);
    local selected = editsystem:GetSelected();
    if type(selected) == "userdata" then
      self.autoreflection:OnGui(selected, timespan);
    elseif type(selected) == "string" then
      assetsGui.OnGui(selected, timespan);
    end
    ImGui.PopStyleVar() -- ImGuiStyleVar_IndentSpacing
    
    imguifunction.ImGui.End();  
  end
end

function inspector:RegisterPanel(panel)
  local tn = panel.PanelName();
  local func = panel.OnGui;
  if tn and func then
    self.autoreflection:RegisterPanel(tn, func);
  else
    ERROR("not a valid panel type")
  end    
end

function inspector:GetTitleName()
  return self.titleName;
end

function inspector:IsOnGui()
  return self.isOnGui;
end

function inspector:SetOnGui(isOnGui)
  self.isOnGui = isOnGui;
end

return inspector;
