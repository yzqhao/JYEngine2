
local defined = require "window.editor.system.defined"
local Math = require "Math"
local Engine = require "Engine"
local MeteorCore = require "MeteorCore"
local Object = require("MeteorCore.core.Object")
local imguifunction = require "imguifunction"

local ImGui = imguifunction.ImGui;

local game = class("game", Object)

function game:ctor()  
  self.titleName = "Game";
  self.isOnGui = true;
  self.ratio = 16/9;
end

function game:OnGui()

end

return game;




