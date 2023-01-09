--[[
    desc: class TransformComponent wrap
    author:{hjy}
    time:2022-09-29 15:51:08
]]

require("functions")

local Engine = require("Engine")
local Component = require("framework.engine.Component")
local LightComponent = class("LightComponent", Component)

function LightComponent:ctor(node)
    LightComponent.super.ctor(self, node)
end

function LightComponent:GetInternal()
    return Engine.ToComLight(self.Internal_)
end

return LightComponent