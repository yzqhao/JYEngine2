
local Math = require "Math"
local Engine = require "Engine"
local Core = require "MeteorCore"
local Object = require "MeteorCore.core.Object"

local Command = class("Command", Object)

function Command:ctor()
  --命令是否处于正在执行过程中
  self.IsExcuting = false;  
  --是否是同步类型的命令
  self.IsSyncCommand = false;
  self.IsExcutInGame = false;
end

function Command:SetSceneID(sceneID)
  self.sceneID = sceneID;
end

function Command:GetSceneID()
  return self.sceneID;
end

function Command:GetScene()
  return Core.SceneManager:Instance():GetSceneById(self.sceneID);
end

function Command:SetSyncType()
  self.IsSyncCommand = true;
end

function Command:IsSyncType()
  return self.IsSyncCommand;
end

function Command:BeginCmd()
  self.IsExcuting = true;
end

function Command:EndCmd()
  self.IsExcuting = false;
end

--像其他场景同步时候的回调
function Command:Sync()
  LOGI("Default Sync");
end

--命令是否执行完成
function Command:IsFinished()
  return not self.IsExcuting;
end

return Command;
