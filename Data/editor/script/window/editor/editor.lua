local mainmenu = require "window.editor.widget.mainmenu.mainmenu"
local windowindex = require "window.windowindex"
local imguifunction = require "imguifunction"
local Math = require "Math"
local MeteorCore = require "MeteorCore"
local EditorSystem = require "window.editor.system.editsystem"
local WindowSystem = require "window.editor.system.windowSystem"
local Object = require("MeteorCore.core.Object")


local ImGui = imguifunction.ImGui;

local editor = class("editor", Object)

local dw = 1280;
local dh = 720;


function editor:ctor()
  self.loadResource = false;
end


function editor:ShowMainWindows()
  self.window = MeteorCore.IApplication:Instance():GetMainWindow();
  self.window:ShowWindow();
  LOGI("Windows system window size: " .. tostring(dw) .. "    " .. tostring(dh));
  self.window:SetWindowSize(dw, dh);
  local wx, wy, ww, wh = self.window:GetMonitorInfomation(0);
  local bx = (ww - dw) / 2;
  local by = (wh - dh) / 2;
  LOGI("Windows system window pos:" .. tostring(bx) .. "    " .. tostring(by));
  self.window:SetWindowPosition(bx, by);
  self.window:MaximizeWindow();
end

function editor:Show(windowData,maxize)
  self.ProjectManager = windowData["ProjectManager"];
  self.mainmenu = mainmenu.new();
  
  --这里的顺序很难搞啊？
  --PluginSystem必须在WndowsSystem之后，EditorSystem又必须在WndowsSystem之前
  EditorSystem:Initialize();
  WindowSystem:Initialize(self.ProjectManager);
  --编辑器启动时候，初始化后打开场景
  self.loadResource = true;

  EditorSystem:OpenScene(self.ProjectManager.sceneFile,
          function()
            if maxize == true then
              self:ShowMainWindows();
            end
            self.loadResource = false;
          end
  );
  return true;

end

function editor:Update(timespan)
  local windowData = {}
  windowData["Index"] = windowindex.editor;
  windowData["Data"] = {};
  if self.loadResource then
    --[[local viewport = ImGui.GetMainViewport();
    ImGui.SetNextWindowPos(viewport.Pos);
    if viewport.Size.x < 500 or viewport.Size.y < 500 then
      ImGui.SetNextWindowSize(imguifunction.ImVec2(500,500));
    else
      ImGui.SetNextWindowSize(viewport.Size);
    end
    ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowPadding, imguifunction.ImVec2(0,0));
    ImGui.Begin("Load Resource ...", true, ImGui.ImGuiWindowFlags_NoScrollbar
            + ImGui.ImGuiWindowFlags_NoBringToFrontOnFocus
            + ImGui.ImGuiWindowFlags_NoDocking
    );
    ImGui.OpenPopup("Load Resource(fbx,hdr)");
    if ImGui.BeginPopupModal("Load Resource(fbx,hdr)", true
                    , ImGui.ImGuiWindowFlags_NoMove
                    + ImGui.ImGuiWindowFlags_NoScrollbar
    ) then
      ImGui.CircleLoadingBar();
      ImGui.Separator();
      ImGui.Text("It may takes a long time...");
      ImGui.EndPopup();
    end
    ImGui.End();
    ImGui.PopStyleVar();]]
    return windowData;
  end
  if self:_Docking(timespan) then
    self.loadResource = true;
    return windowData;
  end
  WindowSystem:Update(timespan);
  EditorSystem:Update(timespan);
  local scene = EditorSystem:GetEditScene();
  --local physics = scene:GetPhysicWorld();
  --physics:UpdateDebug(timespan, 1, 1/30);
  windowData["Data"]["ShouldReloadEditorService"] = EditorSystem:ShouldReloadEditorService(); 
  return windowData;
end

function editor:_Docking(timespan)
  local viewport = ImGui.GetMainViewport();
  ImGui.SetNextWindowPos(viewport.Pos);
  ImGui.SetNextWindowSize(viewport.Size);
  ImGui.SetNextWindowViewport(viewport.ID);
  ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowRounding, 0);
  ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowBorderSize, 0);
  local window_flags = ImGui.ImGuiWindowFlags_MenuBar
    + ImGui.ImGuiWindowFlags_NoDocking
    + ImGui.ImGuiWindowFlags_NoTitleBar
    + ImGui.ImGuiWindowFlags_NoCollapse
    + ImGui.ImGuiWindowFlags_NoResize
    + ImGui.ImGuiWindowFlags_NoMove
    + ImGui.ImGuiWindowFlags_NoBringToFrontOnFocus
    + ImGui.ImGuiWindowFlags_NoNavFocus;
  
  ImGui.PushStyleVar(ImGui.ImGuiStyleVar_WindowPadding, imguifunction.ImVec2(0,0));
  ImGui.Begin("Venus DockSpace", true, window_flags);
  ImGui.PopStyleVar();
  ImGui.PopStyleVar(2);
  local dockspace_id = ImGui.GetID("Venus DockSpace");
  ImGui.DockSpace(dockspace_id);
  
  local result = self.mainmenu:OnGui(timespan);
  
  ImGui.End();
  return result;
end

function editor:Close()
end

function editor:LoadTempScene()
  local tempScenePath = SystemDirectories["temp"];
  local relative_bundlepath = tempScenePath .. "autosavescene.tmp";
  local bundlepath = MeteorCore.IFileSystem:Instance():PathAssembly(relative_bundlepath);
  LOG(bundlepath);
  local bundlenode = EditorSystem:CreateObjectFrombundle(bundlepath);
  if bundlenode then
    EditorSystem:Select(bundlenode);
    local scene = EditorSystem:GetEditScene();
    local apolloRootNode = scene:GetRootNode();
    apolloRootNode:AttachNode(bundlenode);
  end
end

return editor;