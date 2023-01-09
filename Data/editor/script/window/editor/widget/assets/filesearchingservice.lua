local filelfs = require "window.editor.widget.assets.fileutility"
local Core = require "MeteorCore"
local Engine = require "Engine"
local Object = require("MeteorCore.core.Object")
local EditorSystem = require "window.editor.system.editsystem"

local filesearchingservice = class("filesearchingservice", Object)

filesearchingservice.importedFbxPath = Core.IFileSystem:Instance():PathAssembly("proj:library/importedfbx.json");

function filesearchingservice:Initialize(assetpath,librarypath,sceneId)
  self.assetpath = assetpath;
  self.librarypath = librarypath;
  self.sceneId = sceneId;
  return true;
end


function filesearchingservice:StartSearching(path, scene_path, index)
  g_callbackhandle:CallbackFunction(index, FileTree);
end

return filesearchingservice;