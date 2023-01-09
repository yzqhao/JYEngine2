--[[
    desc: cclass Component wrap
    author:{hjy}
    time:2022-09-28 18:59:35
]]

require("functions")

local Object = require("MeteorCore.core.Object")
local Component = class("Component", Object)

function Component:ctor(node)
    Component.super.ctor(self, node)
    self.hostNode_ = node
    self.Internal_ = nil
    self:InitCppComponent()
end

function Component:GetInternal()
    return self.Internal_
end

--- 创建对应的C++实例
function Component:InitCppComponent()
    self.Internal_ = self.hostNode_:GetInternal():CreateComponent(self.__cname)
end

--- 返回组件所属的GameObject
--- @return GameObject @返回组件所属的GameObject
function Component:HostNode()
    return self.hostNode_
end

return Component