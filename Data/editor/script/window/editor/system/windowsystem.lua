local inspector = require "window.editor.widget.inspector.inspector"
local hierarchy = require "window.editor.widget.hierarchy.hierarchy"
local assets = require "window.editor.widget.assets.assets"
local scene = require "window.editor.widget.scene.scene"
local toolbar = require "window.editor.widget.toolbar.toolbar"
local game = require "window.editor.widget.game.game"

local selectwindow = require "window.editor.widget.selectwindow.selectwindow"
local ObjectSelector = require "window.editor.widget.selectwindow.objectselector"

local WindowSystem = {}

function WindowSystem:Initialize(projectManager)
  self.windowlist = {};
  self.memuWindowlist = {};
  self.functionWindowList = {};
  self._game = game.new();
  
  table.insert(self.memuWindowlist, hierarchy.new());
  table.insert(self.memuWindowlist, inspector.new());
  table.insert(self.memuWindowlist, toolbar.new());
  table.insert(self.memuWindowlist, assets.new(projectManager));
  table.insert(self.memuWindowlist, self._game);
  table.insert(self.memuWindowlist, scene.new());


  ---------------添加managerbus界面-----------------
  GlobalSeclectWindow = selectwindow.new();
  table.insert(self.functionWindowList, GlobalSeclectWindow);
  GlobalObjectSelector = ObjectSelector.new();
  table.insert(self.functionWindowList,GlobalObjectSelector);
  
  self.windowlist = table.append(self.memuWindowlist, self.functionWindowList);
end

function WindowSystem:GetMenuWindowList()
  return self.memuWindowlist;
end

function WindowSystem:Update(timespan)
  for key,value in pairs(self.windowlist) do
    local Window = value;
    if Window then
       Window:OnGui(timespan);
    end
  end
end

function WindowSystem:GetGameWindow()
  return self._game;
end

function WindowSystem:GetStateMachine()
  return self._stateMachine;
end

return WindowSystem;
