--[[
    desc: class CameraComponent wrap
    author:{hjy}
    time:2022-09-28 18:59:35
]]

require("functions")

local Engine = require("Engine")
local Component = require("framework.engine.Component")
local CameraComponent = class("CameraComponent", Component)

function CameraComponent:ctor(node)
    CameraComponent.super.ctor(self, node)
end

function CameraComponent:GetInternal()
    return Engine.ToComCamera(self.Internal_)
end

return CameraComponent