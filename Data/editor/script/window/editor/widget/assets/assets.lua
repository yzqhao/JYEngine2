local imguifunction = require "imguifunction"
local Engine = require "Engine"
local Math = require "Math"
local Core = require "MeteorCore"
local Object = require "MeteorCore.core.Object"
local filelfs = require "window.editor.widget.assets.fileutility"
local EditorSystem = require "window.editor.system.editsystem"
local ProjectManager = require "window.project.projectmanager"
local defined = require "window.editor.system.defined"
local venusjson = require "venusjson"
local panelutility = require "window.editor.widget.inspector.panels.utility"
local filesearchtool = require "window.editor.widget.assets.filesearchtool"

local ImGui = imguifunction.ImGui;

local assets = class("assets", Object)

local openfiledialog = false;

function assets:ctor(projectManager)
  self.ProjectManager = ProjectManager:cast(projectManager);
  self.showmetafile = false;
  self.show_progressbar =false;
  self.openfiledialog = false;
  self.fbodialog = false;
  self.meshdialog = false;
  self.matdialog = false;
  self.frameanidialog = false;
  self.progressbar_filename = "";
  self.filesearching = false;
  self.selectedfbxpath = nil;
  self.selectedObjPath = nil;
  self.importfbxpath = nil;
  self.importObjPath = nil;
  self.hasdropdown = false;
  self.fbx_overwrite_popup = false;
  self.obj_overwrite_popup = false;
  self.filePathCount = {};
  self.buttonSize = imguifunction.ImVec2(120, 0);
  self.projectpath = self.ProjectManager:GetProjectPath(); --得到工程目录
  self.projectpath = filelfs:GetCleanPathForDialog(self.projectpath);
  filelfs:CreateDirectory(self.projectpath);
  Core.IFileSystem:Instance():SetProjPath(self.projectpath);
  SystemDirectories = {};
  SystemDirectories = filelfs:CreateDirectories("proj:");  --创建工程默认文件夹

  Core.IFileSystem:Instance():SetAsstPath(Core.IFileSystem:Instance():PathAssembly("proj:assets/"));
  Core.IFileSystem:Instance():SetDocmPath(Core.IFileSystem:Instance():PathAssembly(SystemDirectories["resource"]));
  --Core.IFileSystem:Instance():SetScriptPath(Core.IFileSystem:Instance():PathAssembly(SystemDirectories["script"]));
  self:AddScriptPath(Core.IFileSystem:Instance():PathAssembly(SystemDirectories["script"]));

  GUIDPool = {};
  FileTree = {};

  EditorSystem:RegisterAssetsPathChange(self, self._OnAssetsPathChange);
  
  self:_LoadProjectConfigs(); --读取layer等工程配置

  self.titleName = "assets";
  self.isOnGui = true;
  
  self.chineseChecked = false;
end


function assets:_OnAssetsPathChange(callback)
  self:GetFileSystemTree(callback);
end

function assets:_LoadProjectConfigs()
  local layerjsonpath = Core.IFileSystem:Instance():PathAssembly("proj:library/layerjson.json");
  if Core.IFileSystem:Instance():isFileExist(layerjsonpath) then
    local layers = venusjson.LaodJsonFile(layerjsonpath);
    for _, value in ipairs(layers["Layers"]) do
      Engine.LayerMask:AddLayer(value);
    end
  end
end

function assets:OnGui(timespan)
  if self.isOnGui then
    local b;
    local dockFlag = ImGui.ImGuiWindowFlags_NoScrollbar + ImGui.ImGuiWindowFlags_NoCollapse
    b,self.isOnGui = ImGui.Begin(self.titleName, true, dockFlag);

    if self.fbx_overwrite_popup then
      self:FbxOverWriteChecking();
    end
    if self.obj_overwrite_popup then
      self:ObjOverWriteChecking();
    end
    --文件结构初始化
    if self.filesearching then
      ImGui.OpenPopup("Initializing");
      if ImGui.BeginPopupModal("Initializing") then
        ImGui.CircleLoadingBar();
        ImGui.Separator();
        ImGui.Text("Initializing...");
        ImGui.EndPopup();
      end
    end
    --展示文件树
    if not self.filesearching then
      ImGui.BeginChild("background");
      --右键菜单
      self:_CheckContextWindow();
      self.hasdropdown = false;
      local assetpath = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["scene_asset"]);
      assetpath = string.sub(assetpath, 1, assetpath:len()-1);
      local filetree = FileTree;
      for i = 2, #FileTree do
        if FileTree[i][1] == assetpath then
          filetree = FileTree[i];
          break;
        end
      end
      self:showdirectory(filetree);

      ImGui.EndChild();
      if not self.hasdropdown then
        self:_ProcessDragDropTarget(assetpath);
      end
    end
    if openfiledialog == true then
      local selectedpath = Core.IApplication:Instance():GetMainWindow():GetSelectedPath();
      if #selectedpath == 0 then
        ImGui.End();
        return;
      end
      openfiledialog = false;
      if filelfs:CheckChinese(selectedpath[1]) then
        self.chineseChecked = true;
      else  
        fbxfunc.FbxTool:CreateAnimationFile(filelfs:ConverToRelativePath(selectedpath[1]));
        filelfs:AppendToFileTree(selectedpath[1]);
      end
    end

    if self.chineseChecked then
      panelutility.ErrorView("Name Error", "Please do not use chinese", function () self.chineseChecked = false; end);
    end

    if self._deletePrefabFail then
      panelutility.ErrorView( "Delete Fail", "prefab in use now",self._deletePrefabFail);
    end

    ImGui.End();
  end
end

function assets:ShowProgressBar(flag)
  self.show_progressbar = flag;
end

function assets:SetFileName(name)
  self.progressbar_filename = name;
end

function assets:_CheckContextWindow(node)
  local assetpath = Core.IFileSystem:Instance():PathAssembly(SystemDirectories["scene_asset"]);
  if node then
    if ImGui.BeginPopupContextItem("asset right click",1) then
      if ImGui.BeginMenu("Create") then
        if ImGui.MenuItem("Animation") then
          Core.IApplication:Instance():GetMainWindow():OpenSaveFileDialog("ani", assetpath, "newanimation");
          openfiledialog = true;
        end
        if ImGui.MenuItem("Fbo") then
          Core.IApplication:Instance():GetMainWindow():OpenSaveFileDialog("fbo", assetpath, "newfbo");
          self.fbodialog = true;
        end
        if ImGui.MenuItem("Mesh") then
          Core.IApplication:Instance():GetMainWindow():OpenSaveFileDialog("dynamicmesh", assetpath, "newmesh");
          self.meshdialog = true;
        end
        if ImGui.MenuItem("Mat") then
          Core.IApplication:Instance():GetMainWindow():OpenSaveFileDialog("mat", assetpath, "newmat");
          self.matdialog = true;
        end
        ImGui.EndMenu();
      end
      if ImGui.MenuItem("Export","",false,true) then
        GlobalExportWindow:Open(node);
      end
      if ImGui.BeginMenu("Import") then
        if ImGui.MenuItem("Fbx") then
          self.importfbxpath = filelfs:GetCurrentDirectory(node);
          Core.IApplication:Instance():GetMainWindow():OpenFileDialog("fbx","");
          self.fbxdialog = true;
        end
        if ImGui.MenuItem("Obj") then
          self.importObjPath = filelfs:GetCurrentDirectory(node);
          Core.IApplication:Instance():GetMainWindow():OpenFileDialog("obj","");
          self.objDialog = true;
        end
        if ImGui.MenuItem("Package") then
          GlobalImportWindow:Open(SystemDirectories["scene_asset"], self.projectpath);
        end
        ImGui.EndMenu();
      end
      if ImGui.MenuItem("Delete") then

        local absPath = node
        local postfix = filelfs:getExtension(absPath)
        local deleteSafely = true

        if postfix == "prefab" then

          local relPath = filelfs:ConverToRelativePath(absPath);

          local scene = nil;
          scene = Engine.SceneManager:GetEditScene();

          local nodeList = {}
          local n = scene:GetRootNode()
          table.insert(nodeList, n)

          local referenceNode = nil
          while next(nodeList) ~= nil do
            local n1 = table.remove(nodeList) -- remove last one
            if n1.PrefabPath == relPath then
              referenceNode = n1
              deleteSafely = false
              break
            end
            -- prefab实例下可以再挂一个prefab实例,所以即使当前节点存在PrefabPath还是要找子节点遍历
            local count = n1:GetChildCount();
            local nodes = n1:GetChildrens();
            for _, n2 in pairs(nodes) do
              table.insert(nodeList, n2)
            end
          end

          if deleteSafely then
            
          else
            self._deletePrefabFail =  function()
              self._deletePrefabFail = nil
              EditorSystem:Select(referenceNode)
            end
            -- 显示找到的一个依赖(目前不做列举)
            LOGE("Prefab Delete Fail");
          end
        end

        if deleteSafely then
          os.remove(node);
          os.remove(node .. ".meta");
        end

      end
      if ImGui.MenuItem("Show in Explore") then
        if _PLATFORM_WINDOWS then
          local winDir = string.gsub(node,[[/]], [[\]]);
          local commandline = "explorer.exe /e,/select," .. winDir;
          os.execute(commandline);
        elseif _PLATFORM_MACOS then
            Core.IApplication:Instance():GetMainWindow():ShowInFolder(node,true);

        else
        
        end
      end
      if ImGui.MenuItem("Refresh") then
        self:GetFileSystemTree();
      end
      ImGui.EndPopup();
    end
  else
    if ImGui.BeginPopupContextWindow() then
      if ImGui.BeginMenu("Create") then
        if ImGui.MenuItem("Animation") then
          Core.IApplication:Instance():GetMainWindow():OpenSaveFileDialog("ani", assetpath, "newanimation");
          openfiledialog = true;
        end
        if ImGui.MenuItem("Mesh") then
          Core.IApplication:Instance():GetMainWindow():OpenSaveFileDialog("dynamicmesh", assetpath, "newmesh");
          self.meshdialog = true;
        end
        if ImGui.MenuItem("Mat") then
          Core.IApplication:Instance():GetMainWindow():OpenSaveFileDialog("mat", assetpath, "newmat");
          self.matdialog = true;
        end
        ImGui.EndMenu();
      end
      if ImGui.BeginMenu("Import") then
        if ImGui.MenuItem("Fbx") then
          self.importfbxpath = assetpath;
          Core.IApplication:Instance():GetMainWindow():OpenFileDialog("fbx","");
          self.fbxdialog = true;
        end
        if ImGui.MenuItem("Obj") then
          self.importObjPath = assetpath;
          Core.IApplication:Instance():GetMainWindow():OpenFileDialog("obj","");
          self.objDialog = true;
        end
        if ImGui.MenuItem("Package") then
          GlobalImportWindow:Open(SystemDirectories["scene_asset"], self.projectpath);
        end
        ImGui.EndMenu();
      end
      if ImGui.MenuItem("Show in Explore") then
        if _PLATFORM_WINDOWS then
          local winDir = string.gsub(assetpath,[[/]], [[\]]);
          local commandline = "explorer.exe /e,/root," .. winDir;
          os.execute(commandline);
        elseif _PLATFORM_MACOS then
          Core.IApplication:Instance():GetMainWindow():ShowInFolder(assetpath,false);
        else
        
        end
      end
      if ImGui.MenuItem("Refresh") then
        self:GetFileSystemTree();
      end
      ImGui.EndPopup();
    end
  end
end

function assets:_ProcessDragDropSource(path)
  if path ~= nil and imguifunction.ImGui.BeginDragDropSource(imguifunction.ImGui.ImGuiDragDropFlags_SourceAllowNullID) then
    local nodeName = path;
    imguifunction.ImGui.TextUnformatted(nodeName);  --拖拽时显示node名称
    imguifunction.ImGui.SetDragDropPayload("assets", path); --将ID传递过去，之后再通过ID查找NODE
    imguifunction.ImGui.EndDragDropSource();
    return true;
  end
  return false;
end

function assets:_ProcessDragDropTarget(path)
  if imguifunction.ImGui.BeginDragDropTarget() then
    if imguifunction.ImGui.IsDragDropPayloadBeingAccepted() then
      imguifunction.ImGui.SetMouseCursor(imguifunction.ImGui.ImGuiMouseCursor_Hand);
    else
      imguifunction.ImGui.SetMouseCursor(imguifunction.ImGui.ImGuiMouseCursor_COUNT); --禁止拖拽
    end
    local payloadAtapter = imguifunction.ImGui.AcceptDragDropPayload("Node");
    if payloadAtapter:IsEmpty() == false then
      local dragNodeID = payloadAtapter:GetData();
      --由ID查找node对象
      local dropNode = EditorSystem:GetNodeByID(dragNodeID);
      if dropNode then

        -- 处理原来已是prefab 再拖入assert应该成为新的prefab(类似拷贝创建一个新的类)
        local isPrefabRoot = dropNode.PrefabPath ~= "";
        local isPrefabChild = dropNode.HostPrefabPath ~= ""  and dropNode.PrefabPath == ""

        -- 把prefab实例的一部分 拖曳到asset目录 成为新的prefab类/文件
        -- 如果prefab实例的一部分(topNodeSub) 保存成prefab文件之后，
        -- 原来topNodeSub存在blueprint，要怎么修改classId? 比如TopNode有个变量类型是这个topNodeSub的ClassID ，怎么处理？
        -- 等待考虑清楚怎么同步之后 才开放吧prefab实例的一部分 保存作为prefab文件
        if not isPrefabChild then

          local nodename = dropNode:GetName();
          nodename = string.gsub(nodename,"%.","_");
          nodename = string.gsub(nodename,"/","_");
          nodename = string.lower(nodename);
          local currentDir =  filelfs:GetCurrentDirectory(path);
          local targetpath = venuscore.IFileSystem:PathAssembly(currentDir .. "/" .. nodename .. ".prefab");
          self.hasdropdown = true;
          targetpath = self:GetNoRepeatFileName(targetpath);

          ---  递归通知当前所有节点准备写入prefab文件
          local NotifyPrepareToPrefabFunc
          NotifyPrepareToPrefabFunc = function(node)
            local blueComp = node:GetComponent(apollocore.Node.CT_BLUEPRINT);
            if blueComp then
              local blueprint = BU.GetIns(blueComp);
              if blueprint then
                local ChangeToPrefabCmd = require "window.editor.command.commands.blueprintcomponent.change_to_prefab"
                local cmd1 = ChangeToPrefabCmd(blueComp:GetContentPath(), 1)
                CommandManager:DoIt(cmd1);
              end
            end
            for _, subNode in pairs(node.ChildNodes) do
              NotifyPrepareToPrefabFunc(subNode)
            end
          end
          NotifyPrepareToPrefabFunc(dropNode)

          local cmd = SetPrefabPathCmd(dropNode:GetContentPath(), filelfs:ConverToRelativePath(targetpath));
          CommandManager:DoIt(cmd);
          --将PrefabPath属性序列化到.prefab里头

          BundleManager:SerializeNode(dropNode, targetpath);
          --更新prefab缓存，路径都用相对路径
          PrefabTool:UpdatePrefabInfo(filelfs:ConverToRelativePath(targetpath),nil);

          ---  递归通知当前所有节点已prefab化了
          local NotifyChangeToPrefabFunc
          NotifyChangeToPrefabFunc = function(node)
            local blueComp = node:GetComponent(apollocore.Node.CT_BLUEPRINT);
            if blueComp then
              local blueprint = BU.GetIns(blueComp);
              if blueprint then
                local ChangeToPrefabCmd = require "window.editor.command.commands.blueprintcomponent.change_to_prefab"
                local cmd1 = ChangeToPrefabCmd(blueComp:GetContentPath(), 2)
                CommandManager:DoIt(cmd1);
              end
            end
            for _, subNode in pairs(node.ChildNodes) do
              NotifyChangeToPrefabFunc(subNode)
            end
          end
          NotifyChangeToPrefabFunc(dropNode)

        else
          ERROR("prefab Sub-node can NOT be prefab file");
        end

      end
    end
    imguifunction.ImGui.EndDragDropTarget();
  end
end

function assets:CheckDrag(path)
  if self:_ProcessDragDropSource(path) == false then
    self:_ProcessDragDropTarget(path);
  end
end

function assets:showdirectory(filetree)
  for i=2, #filetree do
    local filename = nil;
    local childtable = nil;
    if type(filetree[i]) == "table" then
      filename = filetree[i][1];
      childtable = filetree[i];
    else
      filename = filetree[i];
    end
    local filetype = filelfs:getExtension(filename);
    if filetype ~= "meta" or self.showmetafile then
      imguifunction.ImGui.PushID(filename);
      imguifunction.ImGui.AlignTextToFramePadding();
      local is_selected = false;  --标识选中状态
      local selected = EditorSystem:GetSelected();
      if selected and selected == filename then
        is_selected = true;
      end
      local nodeName = filelfs:getFileNameWithPosfix(filename);
      local nodeFlags = defined.TreeNodeFlag + imguifunction.ImGui.ImGuiTreeNodeFlags_NoAutoOpenOnLog;
      if is_selected then
        nodeFlags  = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_Selected;
      end

      local no_children = true;
      if childtable ~= nil then
        no_children = false;
      end
      if no_children == true then --没有子Node,则创建叶子节点
        nodeFlags = nodeFlags + imguifunction.ImGui.ImGuiTreeNodeFlags_Leaf;
      end

      imguifunction.ImGui.AlignTextToFramePadding();
      local opened = imguifunction.ImGui.TreeNodeEx(nodeName,nodeFlags);
      self:CheckDrag(filename);  --处理拖拽
      if imguifunction.ImGui.IsItemHovered() and imguifunction.ImGui.IsMouseDragging() == false then
        if imguifunction.ImGui.IsMouseClicked(0) then
          self.currentID = imguifunction.ImGui.GetID(filename);
        end
        local localID = imguifunction.ImGui.GetID(filename);
        if imguifunction.ImGui.IsMouseReleased(0) and self.currentID == localID then
          EditorSystem:Select(filename); --选中此node
        end
      end
      --在node上右键
      self:_CheckContextWindow(filename);

      if opened == true then
        if no_children == false then  --迭代子节点
          self:showdirectory(childtable);
        end
        imguifunction.ImGui.TreePop();
      end
      imguifunction.ImGui.PopID();
    end
  end
end

function assets:GetFileSystemTree(callback)
  self.filesearching = true;
  filesearchtool:GetFileTree(self, callback);
end

function assets:FileSearchingFinish()
  LOGI("searching finish!");
  self.filesearching = false;
end

function assets:GetTitleName()
  return self.titleName;
end

function assets:IsOnGui()
  return self.isOnGui;
end

function assets:SetOnGui(isOnGui)
  self.isOnGui = isOnGui;
end

function assets:AddScriptPath(path)
  local scriptpath = Core.IFileSystem:Instance():PathAssembly(path);
  local temp = scriptpath .."?.lua;";
  temp = temp .. scriptpath .."?/init.lua;;"
  temp = temp .. scriptpath .."?/?.lua;"
  package.path = temp .. package.path;
end


function assets:GetNoRepeatFileName(targetPath)
  local filePath = targetPath;
  self.filePathCount[targetPath] = self.filePathCount[targetPath] or 1;
  while filelfs:exists(filePath) do
    filePath = filelfs:getFileName(targetPath) .. "(" .. tostring(self.filePathCount[targetPath]) .. ")." .. filelfs:getExtension(targetPath);
    self.filePathCount[targetPath] = self.filePathCount[targetPath] + 1;
  end
  return filePath;
end

return assets;
