local imguifunction = require "imguifunction"
local Engine = require "Engine"
local Math = require "Math"
local Core = require "MeteorCore"
local filelfs = require "window.editor.widget.assets.fileutility"
local EditorSystem = require "window.editor.system.editsystem"
local bit = require("bit")
local defined = require "window.editor.system.defined"
local selectscene = require "window.editor.widget.selectwindow.selectscene"
local inputbox = require "window.editor.widget.selectwindow.inputbox"
local panelutility = require "window.editor.widget.inspector.panels.utility"
local WindowSystem = require "window.editor.system.windowSystem"
local BundleManager = require "window.editor.system.BundleManager"
local Object = require("MeteorCore.core.Object")

local ImGui = imguifunction.ImGui;

local mainmenu = class("mainmenu", Object)

local OPERATE_STATE = { 
                         DoNothing = 0,
                         NewSceneOpenDiolog = 3,
                         NewSceneSaveScene = 5,
                         NewSceneDo = 6,
                         OpenSceneOpenChoseWindow = 7,
                         OpenSceneGetPath = 8,
                         OpenSceneOpenDiolog = 9,
                         OpenSceneLoadScene = 10,
                         OpenSceneDoSave = 11,
                         OpenSceneOpenSaveWindow = 12,
                         ExportSceneOpenWindow = 13,
                         ExportGiftSceneOpenWindow = 14,
                         ExportSceneDo = 15,
                         ExportSceneOpenDiolog = 16,
                         ExportSceneOpenChineseDiolog = 17,
                         ExportSceneOpenInvalidCameraDialog = 18,
                         
 };
 
 local CLOSEAPP_STATE = { 
                         CLOSEAPP_DoNothing = 0,
                         CLOSEAPP_OpenDiolog = 1,
                         CLOSEAPP_OpenSaveWindow= 2,
                         CLOSEAPP_DoSave = 3,
                         CLOSEAPP_DO = 4
                         
 };

function mainmenu:ctor()
  self.curState = OPERATE_STATE.DoNothing;
  self.closeAppState = CLOSEAPP_STATE.CLOSEAPP_DoNothing;
  self.windowFlags = bit.bor(ImGui.ImGuiWindowFlags_NoScrollbar,ImGui.ImGuiWindowFlags_NoResize,ImGui.ImGuiWindowFlags_NoSavedSettings,ImGui.ImGuiWindowFlags_NoDocking);
  self.initDiologPos = false;
  self.selectedPathNULL = false;
  self.exportSceneSuc = false;
  self.exportPathHaveChinese = false;
  
  self.focused = false;
  self.selectSceneDialog = selectscene.new();
  self.createSceneInpuBox = inputbox.new("Create new scene", "newscene");
  self.showSelectSceneDialog = false;
  self.showCreateSceneInputBox = false;

  --????????????????????????????????????
  self.isCameraValid = true;
  self.chineseChecked = false;
end

--????????????
function mainmenu:NewScene()
  self.curState = OPERATE_STATE.NewSceneOpenDiolog;
  self.initDiologPos = true;
end

function mainmenu:_OnSelectScene(scenePath)
  self.selectSceneFile = "";
  if scenePath ~= "" then
    self.selectSceneFile = Core.IFileSystem:Instance():PathAssembly("proj:") .. scenePath;
  end
end

--????????????
function mainmenu:OpenScene()
  self.curState = OPERATE_STATE.OpenSceneOpenChoseWindow;
end

--????????????
function mainmenu:SaveScene()
  BundleManager:SaveScene(EditorSystem.sceneFile);
end

--????????????
function mainmenu:SaveSceneAs()
end

--????????????
function mainmenu:ExportScene()
  self.curState = OPERATE_STATE.ExportSceneOpenWindow;
end

function mainmenu:ExportGiftScene()
  self.curState = OPERATE_STATE.ExportGiftSceneOpenWindow;
end


function mainmenu:_LoadScene()
    EditorSystem:ClearScene();
    -- ?????????????????????????????????
    EditorSystem:OpenScene(self.selectSceneFile, g_callbackhandle.StopResourceLoading);
end

function mainmenu:_DoExportScene(path) 
    local selectedpath;
    if path ~= nil then
      selectedpath = path
    else
      selectedpath = Core.IApplication:Instance():GetMainWindow():GetSelectedPath();
    end
    if #selectedpath ~= 0 then
      local zipName = filelfs:getFileNameWithPosfix( filelfs:getFileName(selectedpath[1]));
      local tempscene = self:_SaveTempScene(zipName);
      --????????????????????????
      local isValid, key, cameraList = EditorSystem:CheckCameraValidity();
      
      local haveChinese = filelfs:CheckChinese(selectedpath[1]);
      if haveChinese then
        self.exportPathHaveChinese = true;
        self.chineseChecked = true;
      elseif isValid == false then
        self.isCameraValid = false;  --camera?????????????????????
      else
        packagetool:Export(tempscene, selectedpath[1]);
        self.selectedPathNULL = false;
        self.exportSceneSuc = true;
      end
    else
      self.selectedPathNULL = true;
      self.exportSceneSuc = false;
    end
end

function mainmenu:_OnNewSceneName(name)
  if name == nil then
    return true, "";
  end

  if filelfs:CheckChinese(name) then
    return false, "?????????????????????";
  end

  local index = string.find(name, "[\\/:*?\"<>|]");
  if index ~= nil then
    return false, "??????????????????????????????";
  end

  local path = Core.IFileSystem:Instance():PathAssembly("proj:assets/") .. name .. '/';
  if filelfs:exists(path) then
    return false, "??????????????????????????????????????????";
  end

  EditorSystem:ClearScene(); --???????????????

  EditorSystem:CreateScene(path, name);

  return true, "";
end

function mainmenu:_OpenSaveDiolog()
  if self.initDiologPos then
      local CursorScreenPos = ImGui.GetCursorScreenPos();
      ImGui.SetNextWindowPos(imguifunction.ImVec2(CursorScreenPos.x + 100,CursorScreenPos.y+100));
      ImGui.SetNextWindowSize(imguifunction.ImVec2(170, 80), ImGui.ImGuiCond_FirstUseEver);
      self.initDiologPos = false;
  end
  
  local b1,show = ImGui.Begin("Save current scene",true,self.windowFlags);
  if show==false then
    if self.curState == OPERATE_STATE.OpenSceneOpenDiolog then
      self.curState = OPERATE_STATE.OpenSceneLoadScene;
    else
      self.curState = OPERATE_STATE.DoNothing;
    end
  else
    local contentRegionAvail = imguifunction.ImGui.GetContentRegionAvail();
    local contentRegionAvailWidth = contentRegionAvail.x;
    local contentRegionAvailHeight = contentRegionAvail.y;
    local rectSize = ImGui.GetItemRectSize();
    local currentCursorPos = ImGui.GetCursorPos();
    ImGui.SetCursorPos(imguifunction.ImVec2(currentCursorPos.x + contentRegionAvailWidth / 6,currentCursorPos.y + contentRegionAvailHeight / 2));
    if ImGui.Button("Save") then
      if self.curState == OPERATE_STATE.NewSceneOpenDiolog then
        self.curState = OPERATE_STATE.NewSceneSaveScene;
      elseif self.curState == OPERATE_STATE.OpenSceneOpenDiolog then
        self.curState = OPERATE_STATE.OpenSceneOpenSaveWindow;
      end
    end
    ImGui.SameLine();
    if ImGui.Button("Not Save") then
      if self.curState == OPERATE_STATE.NewSceneOpenDiolog then
        self.curState = OPERATE_STATE.NewSceneDo;
      elseif self.curState == OPERATE_STATE.OpenSceneOpenDiolog then
        self.curState = OPERATE_STATE.OpenSceneLoadScene;
      end
    end
  end
  
  ImGui.End();
end

function mainmenu:_OpenExportDiolog()
  if self.initDiologPos then
    local CursorScreenPos = ImGui.GetCursorScreenPos();
    ImGui.SetNextWindowPos(imguifunction.ImVec2(CursorScreenPos.x + 100,CursorScreenPos.y+100));
    ImGui.SetNextWindowSize(imguifunction.ImVec2(150, 80), ImGui.ImGuiCond_FirstUseEver);
    self.initDiologPos = false;
  end
  
  local result = self.exportSceneSuc and "Success" or "Fail" ;
  local b1,show = ImGui.Begin("Export "..tostring(result), true, self.windowFlags);
  if show == false then
    self.curState = 0;
    self.exportSceneSuc = false;
  else
    local contentRegionAvail = imguifunction.ImGui.GetContentRegionAvail();
    local contentRegionAvailWidth = contentRegionAvail.x;
    local contentRegionAvailHeight = contentRegionAvail.y;
    local currentCursorPos = ImGui.GetCursorPos();
    ImGui.SetCursorPos(imguifunction.ImVec2(currentCursorPos.x + contentRegionAvailWidth / 6,currentCursorPos.y + contentRegionAvailHeight / 2));
    if ImGui.Button("Ok ") then
      self.curState = 0;
      self.exportSceneSuc = false;
    end
  end
  
  ImGui.End();
end


function mainmenu:ShowFileMenu()
  if ImGui.BeginMenu("File")then
    if self.focused then
      if ImGui.MenuItem("New Scene") then  
        --????????????
        self:NewScene();
      end
      
      if ImGui.MenuItem("Open Scene") then
        --????????????
        self:OpenScene();
      end
      
      if ImGui.MenuItem("Save Scene") then
        --????????????
        self:SaveScene();
      end

      if ImGui.MenuItem("Export Scene") then
        self:ExportScene();
      end
      
      if ImGui.MenuItem("Exprot Gift Scene") then
        self:ExportGiftScene();
      end
      

      if ImGui.MenuItem("Export Root") then
        -- ?????? venusroot
        exportsubmenu:onSelected();
      end
    else
      ImGui.CloseCurrentPopup();
    end
    ImGui.EndMenu();
  end

  if self.curState == OPERATE_STATE.NewSceneOpenDiolog then  --????????????
    self:_OpenSaveDiolog();
  elseif self.curState == OPERATE_STATE.NewSceneSaveScene then  --????????????
    BundleManager:SaveScene(EditorSystem.sceneFile);          --?????????????????????????????????????????????
    self.curState = OPERATE_STATE.NewSceneDo;
  elseif self.curState == OPERATE_STATE.NewSceneDo then  --????????????
    self.showCreateSceneInputBox = true;
    self.curState = OPERATE_STATE.DoNothing;
  elseif self.curState == OPERATE_STATE.OpenSceneOpenChoseWindow then  --????????????
    self.showSelectSceneDialog = true;
    self.curState = OPERATE_STATE.OpenSceneGetPath;
  elseif self.curState == OPERATE_STATE.OpenSceneGetPath then  --????????????
    if self.selectSceneFile ~= nil then
      if self.selectSceneFile ~= "" then
        self.curState = OPERATE_STATE.OpenSceneOpenDiolog;
        self.initDiologPos = true;
      else
        self.curState = OPERATE_STATE.DoNothing;
      end
    end
  elseif self.curState == OPERATE_STATE.OpenSceneOpenDiolog then  --????????????,?????????????????????????????????dialog
    self:_OpenSaveDiolog();
  elseif self.curState == OPERATE_STATE.OpenSceneLoadScene then  --????????????,?????????????????????
    --????????????
    self:_LoadScene();
    self.selectSceneFile = nil;
    self.curState = OPERATE_STATE.DoNothing;
    return true;
  elseif self.curState == OPERATE_STATE.OpenSceneDoSave then  --????????????,??????????????????,????????????
    BundleManager:SaveScene(EditorSystem.sceneFile);
    self:_LoadScene();
    self.selectSceneFile = nil;
    self.curState = OPERATE_STATE.DoNothing;
    return true;
  elseif self.curState == OPERATE_STATE.OpenSceneOpenSaveWindow then  --????????????,??????????????????
    self.curState = OPERATE_STATE.OpenSceneDoSave;
  elseif self.curState == OPERATE_STATE.ExportSceneOpenWindow then      --????????????--??????window
    -- local sceneName = "currentscene" .. os.date("%Y%m%d%H%M%S", os.time());
    -- local tempscene = self:_SaveTempScene(sceneName);
    -- GlobalExportWindow:Open(tempscene);
    local isValid, key, cameraList = EditorSystem:CheckCameraValidity();
    if isValid then
      BundleManager:SaveScene(EditorSystem.sceneFile);
      GlobalExportWindow:Open(EditorSystem.sceneFile);
    else
      self.isCameraValid = false;
    end
    self.curState = OPERATE_STATE.DoNothing;
  elseif self.curState == OPERATE_STATE.ExportGiftSceneOpenWindow then
    local isValid, key, cameraList = EditorSystem:CheckCameraValidity();
    if isValid then
      BundleManager:SaveGiftScene(EditorSystem.sceneFile);
      GlobalExportWindow:Open(EditorSystem.sceneFile,true);
    else
      self.isCameraValid = false;
    end
    self.curState = OPERATE_STATE.DoNothing;
  end
  return false;
end

function mainmenu:ShowEditMenu()
  if ImGui.BeginMenu("Edit") then
    if self.focused then
      if ImGui.MenuItem("Undo","CTRL+Z",false,true) then
      end
      if ImGui.MenuItem("Redo", "CTRL+Y", false, false) then
      end
      if ImGui.MenuItem("Cut", "CTRL+X",false,true) then
      end
      if ImGui.MenuItem("Copy", "CTRL+C",false,true) then
      end
      if ImGui.MenuItem("Paste", "CTRL+V",false,true) then
      end
    else
      ImGui.CloseCurrentPopup();
    end
    ImGui.EndMenu();
  end
end

function mainmenu:ShowWindowsMenu()
  local WindowList = WindowSystem:GetMenuWindowList();
  if ImGui.BeginMenu("Windows") then
    if self.focused then
      for key,value in pairs(WindowList) do
          local window = value;
          local titleName = window:GetTitleName();
        if titleName ~= "Select" and titleName ~= "Animation" then
          local status = window:IsOnGui();
          local opened, selected = ImGui.MenuItem(titleName,"",status,true);
          if opened then
               window:SetOnGui(selected);
          end
        end
      end
    else
      ImGui.CloseCurrentPopup();
    end
    ImGui.EndMenu();
  end
end

function mainmenu:_OpenAppCloseSaveDiolog()
  if self.initDiologPos then
    local CursorScreenPos = ImGui.GetCursorScreenPos();
    ImGui.SetNextWindowPos(imguifunction.ImVec2(CursorScreenPos.x + 400,CursorScreenPos.y+400));
    ImGui.SetNextWindowSize(imguifunction.ImVec2(200, 100), ImGui.ImGuiCond_FirstUseEver);
    self.initDiologPos = false;
  end
  
  local b1,show = ImGui.Begin("Save current scene? ",true,self.windowFlags);
  if show==false then
    self.closeAppState = CLOSEAPP_STATE.CLOSEAPP_DoNothing;
    Core.IApplication:Instance():GetMainWindow():SetWindowShoudClose(false);
  else
    local contentRegionAvail = imguifunction.ImGui.GetContentRegionAvail();
    local contentRegionAvailWidth = contentRegionAvail.x;
    local contentRegionAvailHeight = contentRegionAvail.y;
    local rectSize = ImGui.GetItemRectSize();
    local currentCursorPos = ImGui.GetCursorPos();
    ImGui.SetCursorPos(imguifunction.ImVec2(currentCursorPos.x + contentRegionAvailWidth / 6,currentCursorPos.y + contentRegionAvailHeight / 2));
    if ImGui.Button("Save") then
      self.closeAppState = CLOSEAPP_STATE.CLOSEAPP_OpenSaveWindow;
    end
    ImGui.SameLine();
    if ImGui.Button("Not Save") then
      self.closeAppState = CLOSEAPP_STATE.CLOSEAPP_DO;
    end
  end
   
  ImGui.End();
end

function mainmenu:ShowCloseAppMenu()
  local closeApp = Core.IApplication:Instance():GetMainWindow():GetWindowShoudClose();
  if closeApp and self.closeAppState == CLOSEAPP_STATE.CLOSEAPP_DoNothing then
    self.initDiologPos = true;
    self.closeAppState = CLOSEAPP_STATE.CLOSEAPP_OpenDiolog;
  end
  
  --??????????????????????????????dialog??????->??????->?????????
  --                            ???->??????->?????????
  --                            ???->??????
  if self.closeAppState == CLOSEAPP_STATE.CLOSEAPP_OpenDiolog then
    self:_OpenAppCloseSaveDiolog();
  elseif self.closeAppState == CLOSEAPP_STATE.CLOSEAPP_OpenSaveWindow then
    self.closeAppState = CLOSEAPP_STATE.CLOSEAPP_DoSave;
  elseif self.closeAppState == CLOSEAPP_STATE.CLOSEAPP_DoSave then
    BundleManager:SaveScene(EditorSystem.sceneFile);          --?????????????????????????????????????????????
    self.closeAppState = CLOSEAPP_STATE.CLOSEAPP_DO;
  elseif self.closeAppState == CLOSEAPP_STATE.CLOSEAPP_DO then
    Core.IApplication:Instance():GetMainWindow():SetWindowCloseNow(true);
  end

end

function mainmenu:ShowExtraToolsMenu()
  if ImGui.BeginMenu("ExtraTools") then
    if self.focused then
      local opened, selected = ImGui.MenuItem("ShowHideItems","",EditorSystem:ShowEverything(),true);
      if opened then
        EditorSystem:SetShowStaus(selected);
      end
      local opened, selected = ImGui.MenuItem("SerialzeByPrefab","",EditorSystem:GetSerializeByPrefab(),true);
      if opened then
        EditorSystem:SetSerializeByPrefab(selected);
      end
    else
      ImGui.CloseCurrentPopup();
    end
    ImGui.EndMenu();
  end
end

function mainmenu:OnGui(timespan)
  local result = false;
  if imguifunction.ImGui.BeginMenuBar() then
    self.focused = Core.IApplication:Instance():GetMainWindow():GetFocus();

    self:ShowEditMenu();
    
    self:ShowWindowsMenu();  
    
    self:ShowCloseAppMenu();

    if self.chineseChecked then
      panelutility.ErrorView("Name Error", "Please do not use chinese", function () self.chineseChecked = false  end);
    end

    if not self.isCameraValid then
      panelutility.ErrorView("Camera Error", "Please keep active cameras with different Sequence", function ()  self.isCameraValid = true  end);
    end
    self:ShowExtraToolsMenu();

    imguifunction.ImGui.EndMenuBar();
  end

  if self.showSelectSceneDialog then
    self.showSelectSceneDialog = false;
    self.selectSceneDialog:Open(self, self._OnSelectScene);
  end
  
  self.selectSceneDialog:Draw();

  if self.showCreateSceneInputBox then
    self.showCreateSceneInputBox = false;
    self.createSceneInpuBox:Open(self, self._OnNewSceneName);
  end

  self.createSceneInpuBox:Draw();
  return result;
end


function mainmenu:_SaveTempScene(zipName)
  local relativeScenePath = defined.ExportSceneSavePath .. zipName .. ".scene";
  local scenePath = Core.IFileSystem:Instance():PathAssembly(relativeScenePath);
  return scenePath;
end


return mainmenu;