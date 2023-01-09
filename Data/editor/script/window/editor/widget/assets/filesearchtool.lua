local Core = require "MeteorCore"
local EditSystem = require "window.editor.system.editsystem"
local filesearchingservice = require "window.editor.widget.assets.filesearchingservice"

local filesearchtool = {};

function filesearchtool:GetFileTree(window_handle, callback)
  self.filesearchingservice = filesearchingservice.new();
  self.filesearchingservice:Initialize(SystemDirectories["assets"],SystemDirectories["library"], EditSystem:GetUISceneID());
  self.callbackfunction = function(filetree)
    FileTree = filetree;
    if window_handle ~= nil then
      window_handle:FileSearchingFinish();
    end
    if callback ~= nil then
      callback();
    end
    return true;
  end
  self.index = g_callbackhandle:RegisterCallback(self.callbackfunction);
  local path = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["assets"]);
  local scene_path = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["scene_asset"]);
  self.filesearchingservice:StartSearching(path, scene_path, self.index);
  local kkk = 3;
end

return filesearchtool;