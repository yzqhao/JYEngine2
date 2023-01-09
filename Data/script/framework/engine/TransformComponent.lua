--[[
    desc: class TransformComponent wrap
    author:{hjy}
    time:2022-09-29 15:51:08
]]

require("functions")

local Engine = require("Engine")
local Component = require("framework.engine.Component")
local TransformComponent = class("TransformComponent", Component)

function TransformComponent:ctor(node)
    TransformComponent.super.ctor(self, node)
end

function TransformComponent:GetInternal()
    return Engine.ToComTransform(self.Internal_)
end

return TransformComponent