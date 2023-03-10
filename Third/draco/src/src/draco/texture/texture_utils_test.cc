// Copyright 2019 The Draco Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "draco/texture/texture_utils.h"

#ifdef DRACO_TRANSCODER_SUPPORTED
#include "draco/core/draco_test_utils.h"
#include "draco/io/texture_io.h"
#include "draco/texture/color.h"

namespace {

TEST(TextureUtilsTest, TestGetTargetNameForTextureLoadedFromFile) {
  // Tests that correct target stem and format are returned by texture utils for
  // texture loaded from image file (stem and format from source file).
  std::unique_ptr<draco::Texture> texture =
      draco::ReadTextureFromFile(draco::GetTestFileFullPath("fast.jpg"))
          .value();
  ASSERT_NE(texture, nullptr);
  ASSERT_EQ(draco::TextureUtils::GetTargetStem(*texture), "fast");
  ASSERT_EQ(draco::TextureUtils::GetTargetExtension(*texture), "jpg");
  ASSERT_EQ(draco::TextureUtils::GetTargetFormat(*texture),
            draco::ImageFormat::JPEG);
  ASSERT_EQ(draco::TextureUtils::GetOrGenerateTargetStem(*texture, 5, "_Color"),
            "fast");
}

TEST(TextureUtilsTest, TestGetTargetNameForNewTexture) {
  // Tests that correct target stem and format are returned by texture utils for
  // a newly created texture (empty stem and PNG image type by default).
  std::unique_ptr<draco::Texture> texture(new draco::Texture());
  ASSERT_NE(texture, nullptr);
  ASSERT_EQ(draco::TextureUtils::GetTargetStem(*texture), "");
  ASSERT_EQ(draco::TextureUtils::GetOrGenerateTargetStem(*texture, 5, "_Color"),
            "Texture5_Color");
  ASSERT_EQ(draco::TextureUtils::GetTargetExtension(*texture), "png");
  ASSERT_EQ(draco::TextureUtils::GetTargetFormat(*texture),
            draco::ImageFormat::PNG);
}

TEST(TextureUtilsTest, TestGetSourceFormat) {
  // Tests that the source format is determined correctly for new textures and
  // for textures loaded from file.
  std::unique_ptr<draco::Texture> new_texture(new draco::Texture());
  DRACO_ASSIGN_OR_ASSERT(
      std::unique_ptr<draco::Texture> png_texture,
      draco::ReadTextureFromFile(draco::GetTestFileFullPath("test.png")));
  DRACO_ASSIGN_OR_ASSERT(
      std::unique_ptr<draco::Texture> jpg_texture,
      draco::ReadTextureFromFile(draco::GetTestFileFullPath("fast.jpg")));

  // Check source formats.
  ASSERT_EQ(draco::TextureUtils::GetSourceFormat(*new_texture),
            draco::ImageFormat::PNG);
  ASSERT_EQ(draco::TextureUtils::GetSourceFormat(*png_texture),
            draco::ImageFormat::PNG);
  ASSERT_EQ(draco::TextureUtils::GetSourceFormat(*jpg_texture),
            draco::ImageFormat::JPEG);

  // Remove the mime-type from the jpeg texture and ensure the source format is
  // still detected properly based on the filename.
  jpg_texture->source_image().set_mime_type("");
  ASSERT_EQ(draco::TextureUtils::GetSourceFormat(*jpg_texture),
            draco::ImageFormat::JPEG);
}

TEST(TextureUtilsTest, TestGetFormat) {
  typedef draco::ImageFormat ImageFormat;
  ASSERT_EQ(draco::TextureUtils::GetFormat("png"), ImageFormat::PNG);
  ASSERT_EQ(draco::TextureUtils::GetFormat("jpg"), ImageFormat::JPEG);
  ASSERT_EQ(draco::TextureUtils::GetFormat("jpeg"), ImageFormat::JPEG);
  ASSERT_EQ(draco::TextureUtils::GetFormat("basis"), ImageFormat::BASIS);
  ASSERT_EQ(draco::TextureUtils::GetFormat("ktx2"), ImageFormat::BASIS);
  ASSERT_EQ(draco::TextureUtils::GetFormat("webp"), ImageFormat::WEBP);
  ASSERT_EQ(draco::TextureUtils::GetFormat(""), ImageFormat::NONE);
  ASSERT_EQ(draco::TextureUtils::GetFormat("bmp"), ImageFormat::NONE);
}

TEST(TextureUtilsTest, TestGetExtension) {
  typedef draco::ImageFormat ImageFormat;
  ASSERT_EQ(draco::TextureUtils::GetExtension(ImageFormat::PNG), "png");
  ASSERT_EQ(draco::TextureUtils::GetExtension(ImageFormat::JPEG), "jpg");
  ASSERT_EQ(draco::TextureUtils::GetExtension(ImageFormat::BASIS), "ktx2");
  ASSERT_EQ(draco::TextureUtils::GetExtension(ImageFormat::WEBP), "webp");
  ASSERT_EQ(draco::TextureUtils::GetExtension(ImageFormat::NONE), "");
}

#ifdef DRACO_UNRLEASED_FEATURES
TEST(TextureUtilsTest, TestHasTargetImageFormat) {
  // Tests that the presence of image format in texture library can be detected.

  // Create test texture library with textures in PNG and JPEG image formats.
  draco::TextureLibrary library;
  library.PushTexture(
      draco::ReadTextureFromFile(draco::GetTestFileFullPath("test.png"))
          .value());
  library.PushTexture(
      draco::ReadTextureFromFile(draco::GetTestFileFullPath("fast.jpg"))
          .value());

  // Check target texture image formats.
  ASSERT_TRUE(draco::TextureUtils::HasTargetImageFormat(
      library, draco::ImageFormat::PNG));
  ASSERT_TRUE(draco::TextureUtils::HasTargetImageFormat(
      library, draco::ImageFormat::JPEG));
  ASSERT_FALSE(draco::TextureUtils::HasTargetImageFormat(
      library, draco::ImageFormat::BASIS));
  ASSERT_FALSE(draco::TextureUtils::HasTargetImageFormat(
      library, draco::ImageFormat::WEBP));

  // Change PNG texture image format to BASIS and check again.
  library.GetTexture(0)->GetMutableCompressionOptions().target_image_format =
      draco::ImageFormat::BASIS;
  ASSERT_FALSE(draco::TextureUtils::HasTargetImageFormat(
      library, draco::ImageFormat::PNG));
  ASSERT_TRUE(draco::TextureUtils::HasTargetImageFormat(
      library, draco::ImageFormat::BASIS));
}

TEST(TextureUtilsTest, TestFindMutableTextures) {
  // Tests that all textures from material library can be found.

  // Read a mesh that has multiple textures of various types.
  std::unique_ptr<draco::Mesh> mesh =
      draco::ReadMeshFromTestFile("SphereAllSame/sphere_texture_all.gltf");
  ASSERT_NE(mesh, nullptr);
  draco::MaterialLibrary &library = mesh->GetMaterialLibrary();
  ASSERT_EQ(library.NumMaterials(), 1);
  const draco::Material &material = *library.GetMaterial(0);
  ASSERT_EQ(material.NumTextureMaps(), 5);
  ASSERT_EQ(library.GetTextureLibrary().NumTextures(), 4);

  // Check that all textures from material library can be found.
  const std::vector<draco::Texture *> textures =
      draco::TextureUtils::FindMutableTextures(&library);
  ASSERT_EQ(textures.size(), 4);
  ASSERT_EQ(textures[0], library.GetTextureLibrary().GetTexture(0));
  ASSERT_EQ(textures[1], library.GetTextureLibrary().GetTexture(1));
  ASSERT_EQ(textures[2], library.GetTextureLibrary().GetTexture(2));
  ASSERT_EQ(textures[3], library.GetTextureLibrary().GetTexture(3));
}

TEST(TextureUtilsTest, TestFindTexturesWithType) {
  // Tests that textures of a given type can be found.

  // Read a mesh that has multiple textures of various types.
  std::unique_ptr<draco::Mesh> mesh =
      draco::ReadMeshFromTestFile("SphereAllSame/sphere_texture_all.gltf");
  ASSERT_NE(mesh, nullptr);
  const draco::MaterialLibrary &library = mesh->GetMaterialLibrary();
  ASSERT_EQ(library.NumMaterials(), 1);
  const draco::Material &material = *library.GetMaterial(0);
  ASSERT_EQ(material.NumTextureMaps(), 5);
  ASSERT_EQ(library.GetTextureLibrary().NumTextures(), 4);

  // Check that occlusion texture can be found.
  {
    const std::vector<const draco::Texture *> textures =
        draco::TextureUtils::FindTextures(draco::TextureMap::AMBIENT_OCCLUSION,
                                          &library);
    ASSERT_EQ(textures.size(), 1);
    ASSERT_EQ(textures[0],
              library.GetMaterial(0)
                  ->GetTextureMapByType(draco::TextureMap::AMBIENT_OCCLUSION)
                  ->texture());
  }

  // Check that metallic texture can be found.
  {
    const std::vector<const draco::Texture *> textures =
        draco::TextureUtils::FindTextures(draco::TextureMap::METALLIC_ROUGHNESS,
                                          &library);
    ASSERT_EQ(textures.size(), 1);
    ASSERT_EQ(textures[0],
              library.GetMaterial(0)
                  ->GetTextureMapByType(draco::TextureMap::METALLIC_ROUGHNESS)
                  ->texture());
  }
}

TEST(TextureUtilsTest, TestFindMutableTexturesWithType) {
  // Tests that textures of a given type can be found.

  // Read a mesh that has multiple textures of various types.
  std::unique_ptr<draco::Mesh> mesh =
      draco::ReadMeshFromTestFile("SphereAllSame/sphere_texture_all.gltf");
  ASSERT_NE(mesh, nullptr);
  draco::MaterialLibrary &library = mesh->GetMaterialLibrary();
  ASSERT_EQ(library.NumMaterials(), 1);
  const draco::Material &material = *library.GetMaterial(0);
  ASSERT_EQ(material.NumTextureMaps(), 5);
  ASSERT_EQ(library.GetTextureLibrary().NumTextures(), 4);

  // Check that occlusion texture can be found.
  {
    const std::vector<draco::Texture *> textures =
        draco::TextureUtils::FindMutableTextures(
            draco::TextureMap::AMBIENT_OCCLUSION, &library);
    ASSERT_EQ(textures.size(), 1);
    ASSERT_EQ(textures[0],
              library.GetMaterial(0)
                  ->GetTextureMapByType(draco::TextureMap::AMBIENT_OCCLUSION)
                  ->texture());
  }

  // Check that metallic texture can be found.
  {
    const std::vector<draco::Texture *> textures =
        draco::TextureUtils::FindMutableTextures(
            draco::TextureMap::METALLIC_ROUGHNESS, &library);
    ASSERT_EQ(textures.size(), 1);
    ASSERT_EQ(textures[0],
              library.GetMaterial(0)
                  ->GetTextureMapByType(draco::TextureMap::METALLIC_ROUGHNESS)
                  ->texture());
  }
}
#endif  // DRACO_UNRLEASED_FEATURES

}  // namespace

#endif  // DRACO_TRANSCODER_SUPPORTED
