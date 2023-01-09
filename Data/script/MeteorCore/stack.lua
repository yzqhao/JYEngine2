
local Object = require "MeteorCore.core.Object"

local Stack = class("Stack", Object)

--简单栈结构
function Stack:ctor()
  self:Init();
end

function Stack:Init()
  self._stackList = {};
end

function Stack:Reset()  
    self:Init()  
end  
  
function Stack:Clear()  
  self:Init();
end  
  
function Stack:Pop()  
  if #self._stackList == 0 then
    return;
  end
  
  return table.remove(self._stackList);
end 

function Stack:Front()
  if #self._stackList == 0 then
    return nil;
  end
  
  return self._stackList[#self._stackList];
end

  
function Stack:Push(t)
  table.insert(self._stackList,t);
end

function Stack:Count()
  return #self._stackList;
end


return Stack;