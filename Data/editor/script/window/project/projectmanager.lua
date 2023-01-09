local lfs = require "lfs"
local Options = require "window.project.projectoptions"
local Core = require("MeteorCore")
local Object = require("MeteorCore.core.Object")
local filelfs = require "window.editor.widget.assets.fileutility"
local EditorSystem = require "window.editor.system.editsystem"

local ProjectManager = class("ProjectManager", Object) --ProjectManager改成一个对象


function ProjectManager:ctor()
  self.Options = Options.new(); --工程属性配置(以后可能有用)
  self.Options:Deserialize();
  self.sceneFile = "";

  local projectPath = self.Options.persistentConfig.ProjectPath;

  if projectPath ~= nil and lfs.attributes(projectPath, "mode") == nil then --工程路径无效
    projectPath = nil;
    self.Options.persistentConfig.ProjectPath = nil;
    self.Options:Serialize();
  end

  self:_InitProjectPath(projectPath)
end

function ProjectManager:GetProjectPath()
  return self.Options.persistentConfig.ProjectPath;
end

function ProjectManager:SelectScene(path)
  if path ~= "" then
    self.sceneFile = Core.IFileSystem:Instance():PathAssembly("proj:") .. path;
  end
end

function ProjectManager:CreateScene(name)
  if name == nil or name == "" then
    return true, "";
  end
  
  if filelfs:CheckChinese(name) then
    return false, "不能使用中文。";
  end

  local index = string.find(name, "[\\/:*?\"<>|]");
  if index ~= nil then
    return false, "不要使用奇怪的符号。";
  end

  local path = Core.IFileSystem:Instance():PathAssembly("proj:assets/") .. name .. '/';
  if filelfs:exists(path) then
    return false, "场景已存在，请使用其它名字。";
  end

  EditorSystem:CreateScene(path, name);
  self.sceneFile = path .. name .. '.scene';

  return true, "";
end

function ProjectManager:CreateProject(path)
  self.Options.persistentConfig.ProjectPath = path;
  self.Options:Serialize();

  self:_InitProjectPath(path)
end


function ProjectManager:_InitProjectPath(path)
  if path ~= nil then
    path = path .. '/';
    Core.IFileSystem:Instance():SetProjPath(path);
    FileTree = {};
    SystemDirectories = filelfs:CreateDirectories("proj:");  --创建工程默认文件夹
    Core.IFileSystem:Instance():GetAsstPath(Core.IFileSystem:Instance():PathAssembly("proj:assets/"));
  end
end

return ProjectManager;