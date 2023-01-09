
local CommandBase = require "window.editor.command.command"
local Math = require "Math"
local Engine = require "Engine"
local Core = require "MeteorCore"
local BundleManager = require "window.editor.system.BundleManager"


local DeserializeSceneCmd = class("DeserializeSceneCmd", CommandBase)

function DeserializeSceneCmd:ctor(filePath)
  DeserializeSceneCmd.super.ctor(self);
  self._bundlePath = Core.IFileSystem:Instance():PathAssembly(filePath);
  self._scene = nil;
end



function DeserializeSceneCmd:DoIt()
  if self._scene == nil then
    self._scene = BundleManager:Deserialize(self._bundlePath, Engine.Scene)
    self._scene = Engine.ToScene(self._scene);
  else
    local previewScene = Engine.SceneManager:GetPreviewScene();
    BundleManager:Deserialize(self._bundlePath, Engine.Scene)
  end
  Engine.SceneManager:InitScene(self._scene);
  return self._scene;
end



function DeserializeSceneCmd:Undo()
  Engine.SceneManager:ClearScene();  --清理场景
end

return DeserializeSceneCmd;
