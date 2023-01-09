local windowindex = require "window.windowindex"
local project = require "window.project.project"
local ProjectManager = require "window.project.projectmanager"
local editorservice = require "window.editorservice"

--require "venusdebug"
local window = {}

function window:Initialize()
  -- _COROUTINES_ON();
  self.windows = {}
  self.index = windowindex.project; --默认启动创建工程文件对话框
  self.windows[windowindex.project] = project.new();
  --local editorservice = CreateService("edsc:window/editorservice.lua");
  self.windows[windowindex.editor] = editorservice.new();
  
  self.windows[self.index]:Show();
end

function window:PushResponse(requestType, requestId, jsonData)
  self.windows[windowindex.editor]:PushResponse(requestType, requestId, jsonData)
end

function window:Update(timespan)
  local windowData = self.windows[self.index]:Update(timespan);
  local newIndex = windowData["Index"];
  local newWindowData = windowData["Data"];
  
  if newIndex ~= self.index then
    self.windows[self.index]:Close();
    local result = self.windows[newIndex]:Show(newWindowData,true);
    self.index = newIndex;
  end
end

return window;