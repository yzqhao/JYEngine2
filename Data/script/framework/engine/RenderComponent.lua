--[[
    desc: class RenderComponent wrap
    author:{hjy}
    time:2022-09-29 15:51:08
]]

require("functions")

local Engine = require("Engine")
local Component = require("framework.engine.Component")
local RenderComponent = class("RenderComponent", Component)

function RenderComponent:ctor(node)
    RenderComponent.super.ctor(self, node)
end

function RenderComponent:GetInternal()
    return Engine.ToComRender(self.Internal_)
end

return RenderComponent