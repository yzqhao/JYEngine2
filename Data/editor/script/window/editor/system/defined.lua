local Engine = require "Engine"
local imguifunction = require "imguifunction"

local ImGui = imguifunction.ImGui;

local defined = {
  MaterialApiLevel = 71,
  EditorStatus =
  {
    STOP = 0,
    PLAY = 1,
    PAUSE = 2,
  },
  FileTypeList =
  {
    Fbx = "fbx",
    Fbo = "fbo",
    Hdr = "hdr",
    Mesh = "mesh",
    DynamicMesh = "dynamicmesh",
    Shader = "shader",
    Bundle = "bundle",
    Prefab = "prefab",
    Mat = "mat",
    FrameAnimation = "frameani",
    Animation = "ani",
    Skeleton = "ske",
    Metadata = "meta",
    Texture = {"jpg", "png", "fbo", "bmp", "tga"}
  },
  EditorWindowName =
  {
    Assets = "assets",
    Hierarchy = "hierarchy",
    Scene = "scene",
    Game = "game",
    Inspector = "inspector"
  },
  ContextAction =
  {
    None = 0,
    Rename = 1,
    Clone = 2
  },
  TreeNodeFlag = ImGui.ImGuiTreeNodeFlags_AllowItemOverlap + ImGui.ImGuiTreeNodeFlags_OpenOnArrow;
  BasicObjects =
  {
    Box = "comm:documents/basicobjects/box.prefab",
    Cone = "comm:documents/basicobjects/cone.prefab",
    Cylinder = "comm:documents/basicobjects/cylinder.prefab",
    Sphere = "comm:documents/basicobjects/sphere.prefab",
    Teapot = "comm:documents/basicobjects/teapot.prefab",
    Torus = "comm:documents/basicobjects/torus.prefab",
    Tube = "comm:documents/basicobjects/tube.prefab",
    Quad = "comm:documents/basicobjects/quad.prefab",
  },
  LightTypes =
  {
    AmbientLight = Engine.LightComponent.LT_AMBIENT,
    DirectionLight = Engine.LightComponent.LT_DIRECTIONAL,
    PointLight = Engine.LightComponent.LT_POINT,
    SpotLight = Engine.LightComponent.LT_SPOT
  },
  --要用到的自定义图片icon
  Icons =
  {
    Translate = "comm:documents/icons/translate.png",
    Rotation = "comm:documents/icons/rotate.png",
    Scale = "comm:documents/icons/scale.png",
    Play = "comm:documents/icons/play.png",
    Pause = "comm:documents/icons/pause.png",
    Stop = "comm:documents/icons/stop.png",
    Grid = "comm:documents/icons/grid.png",
    WireFrame = "comm:documents/icons/wireframe.png",
    Hierarchy_View = "comm:documents/icons/hierarchy_view.png",
    MaterialDefault = "comm:documents/icons/default_material.png",
    ChangePic = "comm:documents/icons/add_pic.png"
  },
  StadardFace = "comm:documents/texture/standardpic/standardface.jpg",
  StadardCat = "eddc:standardcat.jpg",
  StadardBg = "comm:documents/texture/standardpic/standardbg.jpg",
  StandardFace_Head = "comm:documents/texture/standardpic/standardface_head.png",
  StandardHeadZMap = "comm:documents/texture/standardpic/standardheadZmap.png",
  rootpath = "root:",
  ProjectPath = "proj:";
  ScriptPath = "scrs:";
  ResourcePath = "docs:";
  FirstDirectories = {
    scene_asset = "assets/",
    assets = "assets/",
    library = "library/",
    temp = "temp/",
  },
  SecondDirectories = {
    script = "assets/script/",
    resource = "assets/resource/",
  },
  --shaders = "comm:documents/shaders/",
  shaders = "comm:documents/editor_shaders/",
  debugshaders = "comm:documents/debug_shaders/",

  Mouse = {
    Left = 0,
    Right = 1,
    Mid = 2,
  },
  MouseDragThreshold = 1,
  MouseSpeed = 0.05,
  WheelSpeed = 0.1,
  KeyBoardSpeed = 0.01,
  KeyBoardPressRate = 0.01,
  KeyBoardPressDelay = 0.01,

  DefaultMaterial = "comm:documents/material/unlit.mat",
  DefaultFboSettings = [[ 
    {
      "Version" : 2,
      "TargetType" : 1,
      "SwapTarget" : -1,
      "Scale" : [1,1],
      "Size" : [16,16],
      "srgb" : true, 
      "Attachment":
      {
        "0" :	{
              "TextureType" : 2,
              "TextureUseage" : 0,
              "PixelFormat" : 6,
              "TextureWarp" : [1, 1 ],
              "TextureFilter" : [1, 1 ]
            },
        "11" :	{
              "TextureType" : 2,
              "TextureUseage" : 0,
              "PixelFormat" : 10,
              "TextureWarp" : [1, 1 ],
              "TextureFilter" : [1, 1 ]
            }
      }
    }
  ]],
  DefaultMeshSettings = [[
    {
      "Version" : 2,
      "RenderMode" : 4,
      "UseIndex" : true,
      "VertexAttribute" :
      [
        {
          "slot" : 1001,
          "size" : 4
        },
        {
          "slot" : 1002,
          "size" : 2
        },
        {
          "slot" : 1003,
          "size" : 2
        }
      ]
    }
  ]],
  DefaultMatSettings = [[
    {
      "typename" : "MaterialEntity",
      "fieldname" : "",
      "members" : 
      [
        {
          "typename" : "table",
          "fieldname" : "SourceMetadata",
          "members" : 
          [
            {
              "typename" : "",
              "fieldname" : 1,
              "members" :
              [
                {
                  "typename" : "string",
                  "fieldname" : "Path",
                  "value" : "comm:documents/shaders/opaque/unlit.material",
                  "members" : []
                }
              ]
            }
          ]
        }
      ]
    }
  ]],
  CurrentSceneSavePath = "proj:temp/autosavescene.tmp",
  ExportSceneSavePath = "proj:assets/",  --保存场景到这里，不用临时保存的场景
  BlankTexturePath = "comm:documents/texture/material/blank.jpg",
  PixelFormatEnum =
  {
    Engine.PF_AUTO,
    Engine.PF_A8,
    Engine.PF_L8,
    Engine.PF_L8A8,
    Engine.PF_YUV420P,
    Engine.PF_R8G8B8,
    Engine.PF_R5G6B5,
    Engine.PF_R8G8B8A8,
    Engine.PF_R4G4B4A4,
    Engine.PF_DEPTH16,
    Engine.PF_DEPTH32,
    Engine.PF_DEPTH24_STENCIL8,
    --// floating point texture format
    Engine.PF_RGBAFLOAT,
    Engine.PF_RGBAHALF,
    Engine.PF_RG11B10FLOAT,
  },
  PixelFormatName =
  {
    "PF_AUTO",
    "PF_A8",
    "PF_L8",
    "PF_L8A8",
    "PF_YUV420P",
    "PF_R8G8B8",
    "PF_R5G6B5",
    "PF_R8G8B8A8",
    "PF_R4G4B4A4",
    "PF_DEPTH16",
    "PF_DEPTH32",
    "PF_DEPTH24_STENCIL8",
    --// floating point texture format
    "PF_RGBAFLOAT",
    "PF_RGBAHALF",
    "PF_RG11B10FLOAT",
  },

  TextureUsageEnum = 
  {
    Engine.TU_STATIC,
    Engine.TU_READ,
    Engine.TU_WRITE,
    Engine.TU_COMPUTEWRITE,
  },
  TextureUsageName = 
  {
    "TU_STATIC",
    "TU_READ",
    "TU_WRITE",
    "TU_COMPUTEWRITE"
  },
  VertexAttributeSlot =
  {
    Engine.ATTRIBUTE_POSITION,
    Engine.ATTRIBUTE_NORMAL,
    Engine.ATTRIBUTE_TANGENT,
    Engine.ATTRIBUTE_BINORMAL,
    Engine.ATTRIBUTE_COORDNATE0,
    Engine.ATTRIBUTE_COORDNATE1,
    Engine.ATTRIBUTE_COORDNATE2,
    Engine.ATTRIBUTE_COORDNATE3,
    Engine.ATTRIBUTE_COLOR0,
    Engine.ATTRIBUTE_COLOR1,
  },
  VertexAttributeName =
  {
    "ATTRIBUTE_POSITION",
    "ATTRIBUTE_NORMAL",
    "ATTRIBUTE_TANGENT",
    "ATTRIBUTE_BINORMAL",
    "ATTRIBUTE_COORDNATE0",
    "ATTRIBUTE_COORDNATE1",
    "ATTRIBUTE_COORDNATE2",
    "ATTRIBUTE_COORDNATE3",
    "ATTRIBUTE_COLOR0",
    "ATTRIBUTE_COLOR1",
  },
  RenderMode =
  {
    Engine.RenderComponent.RM_POINTS,
    Engine.RenderComponent.RM_LINES,
    Engine.RenderComponent.RM_LINE_LOOP,
    Engine.RenderComponent.RM_LINE_STRIP,
    Engine.RenderComponent.RM_TRIANGLES,
    Engine.RenderComponent.RM_TRIANGLE_STRIP,
  },
  RenderModeName = 
  {
    "RM_POINTS",
    "RM_LINES",
    "RM_LINE_LOOP",
    "RM_LINE_STRIP",
    "RM_TRIANGLES",
    "RM_TRIANGLE_STRIP",
  },
  TextureTypeMap =
  {
    TT_TEXTURE1D = Engine.TT_TEXTURE1D,
    TT_TEXTURE2D =  Engine.TT_TEXTURE2D,
    TT_TEXTURE3D =  Engine.TT_TEXTURE3D,
    TT_TEXTURECUBE =  Engine.TT_TEXTURECUBE,
    TT_TEXTURECUBE_FRONT =  Engine.TT_TEXTURECUBE_FRONT,
    TT_TEXTURECUBE_BACK =  Engine.TT_TEXTURECUBE_BACK,
    TT_TEXTURECUBE_TOP =  Engine.TT_TEXTURECUBE_TOP,
    TT_TEXTURECUBE_BOTTOM =  Engine.TT_TEXTURECUBE_BOTTOM,
    TT_TEXTURECUBE_LEFT =  Engine.TT_TEXTURECUBE_LEFT,
    TT_TEXTURECUBE_RIGHT =  Engine.TT_TEXTURECUBE_RIGHT,
  },
  TextureWrapName =
  {
    "TW_REPEAT",
    "TW_CLAMP_TO_EDGE",
    "TW_MIRRORED_REPEAT",
    "TW_CLAMP_TO_BORDER",
  },
  TextureFilterName =
  {
    "TF_NEAREST",
    "TF_LINEAR",
    "TF_NEAREST_MIPMAP_NEAREST",
    "TF_LINEAR_MIPMAP_NEAREST",
    "TF_NEAREST_MIPMAP_LINEAR",
    "TF_LINEAR_MIPMAP_LINEAR",
  },
  TextureWrap =
  {
    Engine.TW_REPEAT,
    Engine.TW_CLAMP_TO_EDGE,
    Engine.TW_MIRRORED_REPEAT,
    Engine.TW_CLAMP_TO_BORDER,
  },
  TextureFilter =
  {
    Engine.TF_NEAREST,
    Engine.TF_LINEAR,
    Engine.TF_NEAREST_MIPMAP_NEAREST,
    Engine.TF_LINEAR_MIPMAP_NEAREST,
    Engine.TF_NEAREST_MIPMAP_LINEAR,
    Engine.TF_LINEAR_MIPMAP_LINEAR,
  },
  TextureUseage =
  {
    Engine.TU_STATIC,
    Engine.TU_READ,
    Engine.TU_WRITE,
  },
  TextureUseageName =
  {
    "TU_STATIC",
    "TU_READ",
    "TU_WRITE",
  },
  EditorSceneName = "Scene",
  ShaderPath = "comm:documents/",
  --scene 的绘制优先级
  SceneSequence = 
  {
    HIGH = 0,
    MID = 10,
    LOW = 20,
  },
  SerializeVersion = 1,
  CvDeviceColation = 
  { 
    "LAST_QUEUE_TEXTURE"
  },
  ShaderAPIs =
  {
    "D3D11", "GLES2", "GLES31", "OPENGL"
  },

}



return defined;
