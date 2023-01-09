


--向其他场景转发的命令
local CommandBase = require "window.editor.command.command"
local Math = require "Math"
local Engine = require "Engine"
local Core = require "MeteorCore"
local Object = require "MeteorCore.core.Object"
local Stack = require "MeteorCore.stack"

local SyncCommand = class("SyncCommand", CommandBase)

function SyncCommand:ctor(triggerSceneID, command_list)
  SyncCommand.super.ctor(self);
  self._syncCommandStack = Stack.new();
  self._commandList = command_list;
  self.IsSyncCommand = true;
  self._triggerSceneID = triggerSceneID;--trriggerscene是触发同步的Scene,
                                        --用来过滤不用同步到这个scene

  self._currentSceneID = nil;
  self._sceneToCmdList = {};
  
  --将命令放入栈中
  for i = #command_list, 1, -1 do
    self._syncCommandStack:Push(command_list[i]);
  end
end

function SyncCommand:DoIt()
  self:BeginCmd();
  local mainCmd = self._commandList[1];
  if mainCmd then
    --local sceneList = Engine.SceneManager:GetAllScenes();
    local sceneList = Engine.SceneManager:GetSyncSceneList();
    for key, value in pairs(sceneList) do
      if key ~= self._triggerSceneID then
        local stack = Stack.new();        
        for i = #self._commandList, 2, -1 do
          stack:Push(self._commandList[i]);
        end
        self._sceneToCmdList[key] = stack;
        self._currentSceneID = key;
        mainCmd:SetSceneID(key);
        mainCmd:DoIt();
      end
    end
  end
  self:EndCmd();
end

--new command为执行Sync触发的嵌套而来的消息
function SyncCommand:Sync(newCommand)
  local sceneCmdList = self._sceneToCmdList[self._currentSceneID];
  local triggerCmd = sceneCmdList:Pop();
  if triggerCmd == nil or newCommand == nil then
    LOGE("Nest SyncCommand error!!");
  end
  newCommand:SetSceneID(self.self._currentSceneID);
  newCommand:Sync(triggerCmd);  --交给具体的命令，让各个命令自己处理嵌套的消息怎么融合
  return newCommand:DoIt();                              --比如处理Node和Component的ID一致性问题
end


return SyncCommand;
