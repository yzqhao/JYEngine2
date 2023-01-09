
local Core = require "MeteorCore"
local Object = require "MeteorCore.core.Object"
local Stack = require "MeteorCore.stack"
local SyncCommand = require "window.editor.command.commands.sync_command"

local CommandManager = {}

CommandManager.OnExcutedCallback = {};
CommandManager.OnUndoCallback = {};


--如何保证只从一个scene触发？
function CommandManager:Initialize()
  self._undoStack = Stack.new();
  self._redoStack = Stack.new();
  self._isInSync = false;  --是否正在同步其他场景
  self._syncCommandList = {}; --缓存需要同步的命令，如果是多个则为嵌套命令
  self._syncCommand = nil;
end

function CommandManager:SetTriggerSceneID(sceneID)
  self._triggerSceneID = sceneID;
end

--注册回调
--CommandManager只负责场景树的命令
--如果需要Sync到其他地方（比如Preview场景，请注册回调自行处理)
function CommandManager:OnExcuted(func)
 table.insert(CommandManager.OnExcutedCallback,func);
end

function CommandManager:OnUndo(func)
  table.insert(CommandManager.OnUndoCallback,func);
end

function CommandManager:BeginSync()
  if #self._syncCommandList ~= 0 then
    self._syncCommand  = SyncCommand.new(self._triggerSceneID,self._syncCommandList);
  end
  self._isInSync = true;
end

function CommandManager:EndSync()
  self._isInSync = false;
  self._syncCommand = nil;
end

function CommandManager:IsSyncCommand()
  return self._isInSync;
end



--同步给其他场景
--Sync只能特殊处理了
--Example: CreateGenericNode会触发创建Node的消息
--         创建Node的消息里边会触发两个消息：PropertyChange和CeateComponent
--         其中CreateComponent必须保持ID一致，所以必须用缓存起来的消息
--             PopertyChange(._Script)必须新触发消息
function CommandManager:Sync()
  self:BeginSync();
  self._syncCommand:DoIt();
  self:EndSync();
end




function CommandManager:DoIt(command)
  local res = nil;
  if command then
    --非同步命令
    if self:IsSyncCommand() == false then
      
      --消息从哪个Sene触发归CommandManager管
      command:SetSceneID(self._triggerSceneID);
      
      --查询之前的命令是否执行完毕，避免命令嵌套
      local isPreCmdFinished = true;
      local preCmd = self._undoStack:Front();
      if preCmd then
        isPreCmdFinished = preCmd:IsFinished();
      end
      
      --上一次命令执行完毕了并且非Snc命令才放入到undo队列中（避免嵌套）
      if isPreCmdFinished and command:IsSyncType() == false then
        self._undoStack:Push(command);
      end
      
      table.insert(self._syncCommandList,command);  --缓存Sync命令
      --执行命令
      if command.IsExcutInGame == false then --IsExcutInGame为ture时，不在Scene里执行命令
         res = command:DoIt(); 
      end
      
      
      if isPreCmdFinished then
        self:Sync();
        self._syncCommandList = {};  --清空缓存的Sync命令
      end
      
      self:_ClearRedoStack();  --需要将redo stack清空
      
      for i = 1, #CommandManager.OnExcutedCallback do
        CommandManager.OnExcuted[i](command);
      end
    else  --同步到其他场景的命令
      --这里的sync command都是嵌套的命令
      --需要特殊处理下，比如保证创建的Content和触发场景有相同的ID
      res = self._syncCommand:Sync(command);
    end
  else
    LOGE("Invalid command");
  end
  return res;
end

function CommandManager:IsUndoable() 
  return self._undoStack:Count() ~= 0;
end

--TODO: 暂时先不处理
function CommandManager:Undo()
  local res = nil;
  local count = self._undoStack:Count();
  if count ~= 0 then
    local cmd = self._undoStack:Pop();
    if cmd then
      res = cmd:Undo();
      self._redoStack:Push(cmd);
    end
        
    for i = 1, #CommandManager.OnUndoCallback do
      CommandManager.OnUndo[i](cmd);
    end
  else
    LOGI("Nothing to undo!");
  end
  return res;
end

function CommandManager:IsRedoable()
  return self._redoStack:Count() ~= 0;
end

--暂时先不处理
function CommandManager:Redo()
  local res = nil;
  local count = self._redoStack:Count();
  if count ~= 0 then
    local cmd = self._redoStack:Pop();
    if cmd then
      res = cmd:DoIt();
      self._undoStack:Push(cmd);
    end
    
    for i = 1, #CommandManager.OnExcutedCallback do
      CommandManager.OnExcuted[i](cmd);
    end
  else
    LOGI("Noting to redo!");
  end
  return res;
end

function CommandManager:UndoStackSize()
  return self._undoStack:Count();
end

function CommandManager:RedoStackSize()
  return self._redoStack:Count();
end

function CommandManager:_ClearUndoStack()
  self._undoStack:Clear();
end

function CommandManager:_ClearRedoStack()
  self._redoStack:Clear();
end

function CommandManager:Release()
  self._undoStack = nil;
  self._redoStack = nil;
end

CommandManager:Initialize();

return CommandManager;


