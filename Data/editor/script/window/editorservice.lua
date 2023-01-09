
require("functions")
local MeteorCore = require "MeteorCore"
local Engine = require "Engine"
local Object = require("MeteorCore.core.Object")
local Editor = require "window.editor.editor"

local EditorService = class("EditorService", Object)

g_callbackhandle = nil;

function EditorService:ctor()
  g_callbackhandle = self;
  self.servicecallbacks = {}
  self.callbackindex = 1;
  setmetatable(self.servicecallbacks, {__mode = "v"}) --弱引用

  self.editor = Editor.new();
  --Engine.Engine:AddKeyboardCallback("OnKeyboard");

  return true;
end


function EditorService:OnKeyboard(key,status)
  Keyboard:OnKeyboard(key,status);
  return true;
end

function EditorService:Show(windowData,maxize)
  local result = self.editor:Show(windowData,maxize);
  return result;
end

function EditorService:Update(timeInterval)
  local result = self.editor:Update(timeInterval);
  return result;
end


function EditorService:Close()
  self.editor:Close();
  return true;
end

function EditorService:LoadTempScene()
  self.editor:LoadTempScene();
  return true;
end

--以下为固定接口
function EditorService:RegisterCallback(func)
  local index = self.callbackindex;
  self.callbackindex = self.callbackindex + 1;
  self.servicecallbacks[index] = func;
  return index;
end

function EditorService:CallbackFunction(index, ...)
  local func = self.servicecallbacks[index];
  local res = true;
  if func then
    res = func(...);
  end
  return res;
end

function EditorService.StopResourceLoading()
  EditorService.editor.loadResource = false;
end

return EditorService;
