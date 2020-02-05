/* Copyright (c) 2002-2012 Croteam Ltd. 
   Copyright (c) 2020 Sultim Tsyrendashiev
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_GFXLIBRARY_H
#define SE_INCL_GFXLIBRARY_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#ifdef SE1_D3D
#include <d3d8.h>
#endif // SE1_D3D
#include <Engine/Base/Timer.h>
#include <Engine/Base/CTString.h>
#include <Engine/Base/Lists.h>
#include <Engine/Math/Functions.h>
#include <Engine/Graphics/Adapter.h>

#include <Engine/Graphics/OpenGL.h>

#ifdef SE1_VULKAN
#include <Engine/Graphics/Vulkan/VulkanInclude.h>
#include <Engine/Graphics/Vulkan/SvkStaticHashTable.h>
#endif // SE1_VULKAN

#include <Engine/Graphics/Color.h>
#include <Engine/Graphics/Vertex.h>
#include <Engine/Templates/StaticStackArray.cpp>

// common element arrays
extern CStaticStackArray<GFXVertex>   _avtxCommon;
extern CStaticStackArray<GFXTexCoord> _atexCommon;
extern CStaticStackArray<GFXColor>    _acolCommon;
extern CStaticStackArray<INDEX>       _aiCommonElements;
extern CStaticStackArray<INDEX>       _aiCommonQuads;


#include <Engine/Graphics/GFX_wrapper.h>


#define SQRTTABLESIZE   8192
#define SQRTTABLESIZELOG2 13

#define GFX_MAXTEXUNITS (4L) // maximum number of supported texture units for multitexturing
#define GFX_MINSTREAMS  (3L) // minimum number of D3D streams in order to support HW T&L
#define GFX_MAXLAYERS   (5L) // suggested maximum number of multi-passes per one polygon

// D3D vertex for simple draw functions
struct CTVERTEX {
  FLOAT fX,fY,fZ;  // position
  ULONG ulColor;   // color
  FLOAT fU,fV;     // texture coordinates
};
#define D3DFVF_CTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)


#define GAT_VK_INDEX 1

// Gfx API type 
enum GfxAPIType
{
  GAT_NONE = -1,     // no gfx API (gfx functions are disabled)
  GAT_OGL  =  0,     // OpenGL
#ifdef SE1_D3D
  GAT_D3D  =  1,     // Direct3D
#endif // SE1_D3D
#ifdef SE1_VULKAN
  GAT_VK   =  GAT_VK_INDEX,   // Vulkan
#endif // SE1_VULKAN

  GAT_CURRENT = 9,   // current API
};


// vertex type (for lock/unlock function)
enum VtxType
{
  VXT_LOC = 1,  // x,y,z location
  VXT_NOR = 2,  // normal
  VXT_TEX = 3,  // texture coords
  VXT_COL = 4,  // color
};


// common flags
#define GLF_HASACCELERATION    (1UL<<0)   // set if current mode supports hardware acceleration
#define GLF_INITONNEXTWINDOW   (1UL<<1)   // initialize rendering context on next window
#define GLF_ADJUSTABLEGAMMA    (1UL<<2)   // set if display allows gamma adjustment
#define GLF_TEXTURECOMPRESSION (1UL<<3)   // set if texture compression is supported
#define GLF_32BITTEXTURES      (1UL<<4)   // 32-bit textures
#define GLF_VSYNC              (1UL<<5)   // supports wait for VSYNC
#define GLF_FULLSCREEN         (1UL<<9)   // currently in full-screen mode

// Direct3D specific
#define GLF_D3D_HASHWTNL    (1UL<<10)   // supports hardware T&L
#define GLF_D3D_USINGHWTNL  (1UL<<11)   // using hardware T&L
#define GLF_D3D_CLIPPLANE   (1UL<<12)   // supports at least one custom clip plane
#define GLF_D3D_COLORWRITES (1UL<<13)   // supports enable/disable writes to color buffer
#define GLF_D3D_ZBIAS       (1UL<<14)   // supports z-biasing

// OpenGL extensions (part of flags!)
#define GLF_EXT_TBUFFER             (1UL<<19)   // 3DFX's T-Buffer (for partial FSAA & Motion-blur effects)
#define GLF_EXT_EDGECLAMP           (1UL<<20)   // GL_EXT_texture_edge_clamp
#define GLF_EXT_COMPILEDVERTEXARRAY (1UL<<21)   // GL_EXT_compiled_vertex_array
#define GLF_EXT_CLIPHINT            (1UL<<22)   // GL_EXT_clip_volume_hint
#define GLF_EXT_OCCLUSIONTEST       (1UL<<23)   // GL_HP_occlusion_test
#define GLF_EXT_OCCLUSIONQUERY      (1UL<<24)   // GL_NV_occlusion_query
	
#define GLF_EXTC_ARB    (1UL<<27)   // GL_ARB_texture_compression
#define GLF_EXTC_S3TC   (1UL<<28)   // GL_EXT_texture_compression_s3tc
#define GLF_EXTC_FXT1   (1UL<<29)   // GL_3DFX_texture_compression_FXT1
#define GLF_EXTC_LEGACY (1UL<<30)   // GL_S3_s3tc


/*
 *  Graphics library object.
 */
class ENGINE_API CGfxLibrary
{
public:
  CGfxAPI gl_gaAPI[2];
  CViewPort *gl_pvpActive;   // active viewport
  HINSTANCE  gl_hiDriver;    // DLL handle

  GfxAPIType   gl_eCurrentAPI;  // (0=none, 1=OpenGL, 2=DirectX8, 2=Vulkan) 
  CDisplayMode gl_dmCurrentDisplayMode;
  INDEX gl_iCurrentAdapter;
  INDEX gl_iCurrentDepth; 
  INDEX gl_ctDriverChanges;        // count of driver changes
  ULONG gl_ulFlags;

#ifdef SE1_VULKAN
  VkInstance                      gl_VkInstance;
  VkDevice                        gl_VkDevice;
  VkSurfaceKHR                    gl_VkSurface;

  VkSwapchainKHR                  gl_VkSwapchain;
  VkExtent2D                      gl_VkSwapChainExtent;
  uint32_t                        gl_VkCurrentImageIndex;
  VkFormat                        gl_VkSurfColorFormat;
  VkColorSpaceKHR                 gl_VkSurfColorSpace;
  VkFormat                        gl_VkSurfDepthFormat;
  VkPresentModeKHR                gl_VkSurfPresentMode;
  CStaticArray<VkImage>           gl_VkSwapchainImages;
  CStaticArray<VkImageView>       gl_VkSwapchainImageViews;
  CStaticArray<VkImage>           gl_VkSwapchainDepthImages;
  CStaticArray<VkDeviceMemory>    gl_VkSwapchainDepthMemory;
  CStaticArray<VkImageView>       gl_VkSwapchainDepthImageViews;
  CStaticArray<VkFramebuffer>     gl_VkFramebuffers;

  VkSemaphore                     gl_VkImageAvailableSemaphores[gl_VkMaxCmdBufferCount];
  VkSemaphore                     gl_VkRenderFinishedSemaphores[gl_VkMaxCmdBufferCount];
  VkFence                         gl_VkCmdFences[gl_VkMaxCmdBufferCount];

  VkRenderPass                    gl_VkRenderPass;

  VkDescriptorSetLayout           gl_VkDescriptorSetLayout;
  VkDescriptorSetLayout           gl_VkDescSetLayoutTexture;
  VkPipelineLayout                gl_VkPipelineLayout;

  VkShaderModule                  gl_VkShaderModuleVert;
  VkShaderModule                  gl_VkShaderModuleFrag;
  VkShaderModule                  gl_VkShaderModuleFragAlpha;

  VkRect2D                        gl_VkCurrentScissor;
  VkViewport                      gl_VkCurrentViewport;

  uint32_t                        gl_VkCmdBufferCurrent;
  VkCommandPool                   gl_VkCmdPool;
  VkCommandBuffer                 gl_VkCmdBuffers[gl_VkMaxCmdBufferCount];
  bool                            gl_VkCmdIsRecording;

  VkDescriptorPool                        gl_VkDescriptorPool;

  SvkDynamicBufferGlobal                  gl_VkDynamicVBGlobal;
  SvkDynamicBuffer                        gl_VkDynamicVB[gl_VkMaxCmdBufferCount];
 
  SvkDynamicBufferGlobal                  gl_VkDynamicIBGlobal;
  SvkDynamicBuffer                        gl_VkDynamicIB[gl_VkMaxCmdBufferCount];
  
  SvkDynamicBufferGlobal                  gl_VkDynamicUBGlobal;
  SvkDynamicUniform                       gl_VkDynamicUB[gl_VkMaxCmdBufferCount];

  // dynamic buffers to delete
  CStaticStackArray<SvkDBufferToDelete>   gl_VkDynamicToDelete[gl_VkMaxCmdBufferCount];

  SvkSamplerFlags                         gl_VkGlobalSamplerState;
  CStaticStackArray<SvkSampler>           gl_VkSamplers;
  // all loaded textures
  SvkStaticHashTable<SvkTextureObject>    gl_VkTextures;
  uint32_t                                gl_VkLastTextureId;

  CStaticStackArray<uint32_t>             gl_VkTexturesToDelete[gl_VkMaxCmdBufferCount];

  // pointers to currently active textures
  uint32_t                                gl_VkActiveTextures[GFX_MAXTEXUNITS];

  SvkPipelineStateFlags                   gl_VkGlobalState;
  SvkPipelineState                        *gl_VkPreviousPipeline;
  CStaticStackArray<SvkPipelineState>     gl_VkPipelines;
  VkPipelineCache                         gl_VkPipelineCache;
  SvkVertexLayout                         *gl_VkDefaultVertexLayout;

  VkPhysicalDevice                        gl_VkPhysDevice;
  VkPhysicalDeviceMemoryProperties        gl_VkPhMemoryProperties;
  VkPhysicalDeviceProperties              gl_VkPhProperties;
  VkPhysicalDeviceFeatures                gl_VkPhFeatures;
  VkSurfaceCapabilitiesKHR                gl_VkPhSurfCapabilities;
  CStaticArray<VkSurfaceFormatKHR>        gl_VkPhSurfFormats;
  CStaticArray<VkPresentModeKHR>          gl_VkPhSurfPresentModes;

  CStaticArray<const char *>              gl_VkPhysDeviceExtensions;
  CStaticArray<const char *>              gl_VkLayers;

  uint32_t                        gl_VkQueueFamGraphics;
  uint32_t                        gl_VkQueueFamTransfer;
  uint32_t                        gl_VkQueueFamPresent;
  VkQueue                         gl_VkQueueGraphics;
  VkQueue                         gl_VkQueueTransfer;
  VkQueue                         gl_VkQueuePresent;

  VkDebugUtilsMessengerEXT        gl_VkDebugMessenger;

  // current mesh
  CStaticStackArray<SvkVertex>    gl_VkVerts;

  // do not remove this dummy as created window with zero size in ViewPort.cpp 
  // will be NULL without it
  const uint32_t                  gl_VkDummy = 0;

#endif

#ifdef SE1_D3D
  // DirectX info
  LPDIRECT3D8       gl_pD3D;       // used to create the D3DDevice
  LPDIRECT3DDEVICE8 gl_pd3dDevice; // rendering device
  D3DFORMAT gl_d3dColorFormat;     // current color format
  D3DFORMAT gl_d3dDepthFormat;     // z-buffer depth format
  // DirectX vertex/index buffers
  LPDIRECT3DVERTEXBUFFER8 gl_pd3dVtx;
  LPDIRECT3DVERTEXBUFFER8 gl_pd3dNor;
  LPDIRECT3DVERTEXBUFFER8 gl_pd3dCol[GFX_MAXLAYERS];
  LPDIRECT3DVERTEXBUFFER8 gl_pd3dTex[GFX_MAXLAYERS];
  LPDIRECT3DINDEXBUFFER8  gl_pd3dIdx;
#endif // SE1_D3D
  INDEX gl_ctVertices, gl_ctIndices;  // size of buffers
  INDEX gl_ctColBuffers, gl_ctTexBuffers; // number of color and texture buffers (for multi-pass rendering)
  INDEX gl_ctMaxStreams;              // maximum number of streams
  DWORD gl_dwVertexShader;

  // OpenGL info
  HGLRC    go_hglRC;                  // rendering context
  CTString go_strExtensions;          // reported extensions
  CTString go_strWinExtensions;
  CTString go_strSupportedExtensions; // supported extensions
  INDEX go_ctSampleBuffers;           // number of rendering buffers currently on (for T-buffer's motion-blur effect) 0=not supported
  INDEX go_iCurrentWriteBuffer;       // current buffer enabled for writing

  // common stuff
  
  PIX   gl_pixMaxTextureDimension;  // maximum allowed texture dimension by driver
  INDEX gl_iSwapInterval;           // current swap interval (0=immediate, n=VSyncs to wait)
  INDEX gl_ctTextureUnits;          // maximum supported texture units (1=no multitexturing)
  INDEX gl_ctRealTextureUnits;      // maximum reported texture units
  FLOAT gl_fTextureLODBias;         // current texture lod bias
  FLOAT gl_fMaxTextureLODBias;      // absolute maximum possible texture lod bias (0 if biasing is not supported)
  INDEX gl_iMaxTextureAnisotropy;   // maximum possible texture degree of anisotropy (1 if isotropic only)
  INDEX gl_iMaxTessellationLevel;   // maximum possible truform tesselation level (0=no truform)
  INDEX gl_iTessellationLevel;      // current tesselation level

  INDEX gl_iFrameNumber;           // frame number currently rendering 
  CTimerValue gl_tvFrameTime;      // time when swapbuffer occured 
  SLONG gl_slAllowedUploadBurst;   // remain upload burst size for this frame (max texture or shadowmap size *2)
  CListHead gl_lhCachedShadows;    // list of all cached shadowmaps
  CListHead gl_lhRenderTextures;   // list of all render-textures
  BOOL gl_bAllowProbing;

  // for profiling
  INDEX gl_ctWorldTriangles;     // how many world polygons were rendered in last frame
  INDEX gl_ctModelTriangles;     // how many model polygons were rendered in last frame
  INDEX gl_ctParticleTriangles;  // how many particle polygons were rendered in last frame
  INDEX gl_ctTotalTriangles;     // total = (world & lensflares) + (model & shadows) + particle + 2D

private:

  void InitAPIs(void);
  // start full-screen or windowed mode (pixSizeI=pixSizeJ=0 & iAdapter=0 if windowed)
  BOOL StartDisplayMode( enum GfxAPIType eAPI, INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ,
                         enum DisplayDepth eColorDepth);
  void StopDisplayMode(void);

  // OpenGL specific
  BOOL InitDriver_OGL( BOOL b3Dfx=FALSE);  // DLL init and function call adjustments
  void EndDriver_OGL(void);
  void TestExtension_OGL( ULONG ulFlag, const char *strName); // if exist, add OpenGL extension to flag and list
  void AddExtension_OGL(  ULONG ulFlag, const char *strName); // unconditionally add OpenGL extension to flag and list
#ifdef PLATFORM_WIN32
  BOOL CreateContext_OGL( HDC hdc);
  BOOL SetupPixelFormat_OGL( HDC hdc, BOOL bReport=FALSE);
#endif
  void InitContext_OGL(void);
  BOOL SetCurrentViewport_OGL( CViewPort *pvp);
  void UploadPattern_OGL( ULONG ulPatternEven); 
  void SwapBuffers_OGL( CViewPort *pvpToSwap);

  // Direct3D specific
  BOOL InitDriver_D3D(void);
  void EndDriver_D3D(void);
  BOOL InitDisplay_D3D( INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ, enum DisplayDepth eColorDepth);
  void InitContext_D3D(void);
  BOOL SetCurrentViewport_D3D( CViewPort *pvp);
  void UploadPattern_D3D( ULONG ulPatternEven);
  void SwapBuffers_D3D( CViewPort *pvpToSwap);

#ifdef SE1_VULKAN
  // Vulkan specific
  BOOL InitDriver_Vulkan();
  void EndDriver_Vulkan();
  void Reset_Vulkan();
  //BOOL InitDisplay_Vulkan(INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ, enum DisplayDepth eColorDepth);
  void InitContext_Vulkan();
  BOOL SetCurrentViewport_Vulkan(CViewPort* pvp);
  void SwapBuffers_Vulkan();
  void SetViewport_Vulkan(float leftUpperX, float leftUpperY, float width, float height, float minDepth, float maxDepth);

  BOOL PickPhysicalDevice();
  BOOL InitSurface_Win32(HINSTANCE hinstance, HWND hwnd);
  BOOL CreateDevice();
  void CreateRenderPass();

  void CreateSyncPrimitives();
  void DestroySyncPrimitives();

  void CreateVertexLayouts();
  void DestroyVertexLayouts();

  // create desc set layout and its pipeline layout
  void CreateDescriptorSetLayouts();
  void DestroyDescriptorSetLayouts();

  void CreateShaderModules();
  void DestroyShaderModules();

  void CreateDescriptorPools();
  void DestroyDescriptorPools();

  SvkPipelineState &GetPipeline(SvkPipelineStateFlags flags);
  // create new pipeline and add it to list
  SvkPipelineState &CreatePipeline(SvkPipelineStateFlags flags, const SvkVertexLayout &vertLayout,
    VkShaderModule vertShader, VkShaderModule fragShader);
  void CreatePipelineCache();
  void DestroyPipelines();

  BOOL CreateSwapchainDepth(uint32_t width, uint32_t height, uint32_t imageIndex);

  void CreateCmdBuffers();
  void DestroyCmdBuffers();

  void InitDynamicBuffers();
  void InitDynamicVertexBuffers(uint32_t newSize);
  void InitDynamicIndexBuffers(uint32_t newSize);
  void InitDynamicUniformBuffers(uint32_t newSize);
  void InitDynamicBuffer(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicBuffer *buffers, VkBufferUsageFlags usage);
  
  void ClearCurrentDynamicOffsets(uint32_t cmdBufferIndex);
  void GetVertexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer);
  void GetIndexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer);
  void GetUniformBuffer(uint32_t size, SvkDynamicUniform &outDynUniform);
  void FlushDynamicBuffersMemory();

  void AddDynamicBufferToDeletion(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicBuffer *buffers);
  void AddDynamicUniformToDeletion(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicUniform *buffers);
  // free frame data: vertex, index, uniform buffers, descriptor sets
  void FreeUnusedDynamicBuffers(uint32_t cmdBufferIndex);
  // destroy all dynamic buffers data, including unused
  void DestroyDynamicBuffers();

  VkSampler GetSampler(SvkSamplerFlags flags);
  VkSampler CreateSampler(SvkSamplerFlags flags);
  void DestroySamplers();

  void CreateTexturesDataStructure();
  void DestroyTexturesDataStructure();

  SvkTextureObject &GetTextureObject(uint32_t textureId);
  VkDescriptorSet GetTextureDescriptor(uint32_t textureId);
  void FreeDeletedTextures(uint32_t cmdBufferIndex);

  void AcquireNextImage();
  void StartFrame();
  void EndFrame();

  VkShaderModule CreateShaderModule(const uint32_t *spvCode, uint32_t codeSize);

  // utils
  BOOL GetQueues(VkPhysicalDevice physDevice,
    uint32_t &graphicsQueueFamily, uint32_t &transferQueueFamily, uint32_t &presentQueueFamily);
  BOOL CheckDeviceExtensions(VkPhysicalDevice physDevice, const CStaticArray<const char *> &requiredExtensions);
  VkExtent2D GetSwapchainExtent(uint32_t width, uint32_t height);
  uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask);
  uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask, VkFlags preferredMask);
  VkFormat FindSupportedFormat(const VkFormat *formats, uint32_t formatCount, VkImageTiling tiling, VkFormatFeatureFlags features);
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
  void CopyToDeviceMemory(VkDeviceMemory deviceMemory, const void *data, VkDeviceSize size);

public:
  void CreateSwapchain(uint32_t width, uint32_t height);
  void RecreateSwapchain(uint32_t newWidth, uint32_t newHeight);
  void DestroySwapchain();


  // get current global pipeline state
  SvkPipelineStateFlags &GetPipelineState();


  // Get current started cmd buffer to write in
  VkCommandBuffer GetCurrentCmdBuffer();
  void DrawTriangles(uint32_t indexCount, const uint32_t *indices);


  void SetTexture(uint32_t textureUnit, uint32_t textureId, SvkSamplerFlags samplerFlags);
  // create texture handler, texture IDs starts with 1, not 0
  uint32_t CreateTexture();
  // init texture; if onlyUpdate is true, texture will not be allocated
  void InitTexture32Bit(
    uint32_t &textureId, VkFormat format, void *textureData,
    VkExtent2D *mipLevels, uint32_t mipLevelsCount, bool onlyUpdate);
  // delete texture
  void AddTextureToDeletion(uint32_t textureId);
  // for statistics
  uint32_t GetTexturePixCount(uint32_t textureId);


  void ClearColor(int32_t x, int32_t y, uint32_t width, uint32_t height, float *rgba);
  void ClearDepth(int32_t x, int32_t y, uint32_t width, uint32_t height, float depth);
  void ClearColor(float *rgba);
  void ClearDepth(float depth);
#endif // SE1_VULKAN

public:

  // common
  CGfxLibrary();      // init cvars
  ~CGfxLibrary();     // clean up

  // optimize memory used by cached shadow maps
  void ReduceShadows(void);

  // pretouch all textures and cached shadowmaps
  void PretouchIfNeeded(void);

  // get array of all supported display modes (do not modify array!)
  CDisplayMode *EnumDisplayModes( INDEX &ctModes, enum GfxAPIType eAPI=GAT_CURRENT, INDEX iAdapter=0);

  // set viewport for rendering
  BOOL SetCurrentViewport( CViewPort *pvp);
  // swap buffers in a viewport
  void SwapBuffers( CViewPort *pvpToSwap);
  // return current frame number
  inline INDEX GetFrameNumber(void) { return gl_iFrameNumber; };

  // lock/unlock raster for drawing
  BOOL LockRaster(   CRaster *praToLock);
  void UnlockRaster( CRaster *praToUnlock);
  // lock/unlock drawport for drawing
  BOOL LockDrawPort(   CDrawPort *pdpToLock);
  void UnlockDrawPort( CDrawPort *pdpToUnlock);

  // initialize library
  void Init(void);  

  // set new display mode
  BOOL SetDisplayMode( enum GfxAPIType eAPI, INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ,
                       enum DisplayDepth eColorDepth);
  // set display mode to original desktop display mode (and eventually change API)
  BOOL ResetDisplayMode( enum GfxAPIType eAPI=GAT_CURRENT);

  // get current API/adapter/display mode
  inline enum  GfxAPIType GetCurrentAPI(void) { return gl_eCurrentAPI; };
  inline INDEX GetCurrentAdapter(void) { return gl_iCurrentAdapter; };
  inline void  GetCurrentDisplayMode( CDisplayMode &dmCurrent) { dmCurrent = gl_dmCurrentDisplayMode; };

  // check if 3D acceleration is on
  inline BOOL IsCurrentModeAccelerated(void) { return (gl_ulFlags&GLF_HASACCELERATION); };
  // check if DualHead is on
  inline BOOL IsCurrentModeDH(void) { return gl_dmCurrentDisplayMode.IsDualHead(); };

  // is API supported
  inline BOOL HasAPI( enum GfxAPIType eAPI) {
    if( eAPI==GAT_CURRENT) return TRUE;
    if( eAPI==GAT_OGL) return (gl_gaAPI[0].ga_ctAdapters>0);
#ifdef SE1_D3D
    if( eAPI==GAT_D3D) return (gl_gaAPI[1].ga_ctAdapters>0);
#endif // SE1_D3D
#ifdef SE1_VULKAN
    if (eAPI == GAT_VK) return (gl_gaAPI[GAT_VK_INDEX].ga_ctAdapters > 0);
#endif // SE1_VULKAN
    return FALSE;
  };

  // canvas functions
  void CreateWindowCanvas( void *hWnd, CViewPort **ppvpNew, CDrawPort **ppdpNew);   // Create a new window canvas
  void DestroyWindowCanvas( CViewPort *pvpOld);                                     // Destroy a window canvas
  void CreateWorkCanvas( PIX pixWidth, PIX pixHeight, CDrawPort **ppdpNew);         // Create a work canvas
  void DestroyWorkCanvas( CDrawPort *pdpOld);                                       // Destroy a work canvas

  // simple benchmark routine
  void Benchmark( CViewPort *pvp, CDrawPort *pdp);
};


struct MipmapTable {
  INDEX mmt_ctMipmaps;      // number of mip-maps
  SLONG mmt_slTotalSize;    // total size of all mip-maps (in pixels)
  PIX mmt_pixU;             // size of first mip-map
  PIX mmt_pixV;
  SLONG mmt_aslOffsets[MAX_MEX_LOG2]; // offsets of each mip-map (in pixels)
};


// returns number of mip-maps to skip from original texture depending on maximum allowed texture
// size (in pixels!) and maximum allowed texture dimension (in pixels, too!)
extern INDEX ClampTextureSize( PIX pixClampSize, PIX pixClampDimension, PIX pixSizeU, PIX pixSizeV);

// create mip-map table for texture or shadow of given dimensions
extern void MakeMipmapTable( PIX pixU, PIX pixV, MipmapTable &mmt);

// adds 8-bit opaque alpha channel to 24-bit bitmap (in place suported)
extern void AddAlphaChannel( UBYTE *pubSrcBitmap, ULONG *pulDstBitmap, PIX pixSize, UBYTE *pubAlphaBitmap=NULL);
// removes 8-bit alpha channel from 32-bit bitmap (in place suported)
extern void RemoveAlphaChannel( ULONG *pulSrcBitmap, UBYTE *pubDstBitmap, PIX pixSize);

// flips 24 or 32-bit bitmap (iType: 1-horizontal, 2-vertical, 3-diagonal) - in place supported
extern void FlipBitmap( UBYTE *pubSrc, UBYTE *pubDst, PIX pixWidth, PIX pixHeight, INDEX iFlipType, BOOL bAlphaChannel);


// makes ALL lower mipmaps (to size of Nx1 or 1xM) of a specified 32-bit bitmap with eventual filter appliance
// this is done in-place! - all subsequent mip-maps will be posioned just after the original, input bitmap)
// (only first ctFineMips number of mip-maps will be filtered with bilinear subsampling, while
//  all others will be downsampled with nearest-neighbour method with border preservance)
extern void MakeMipmaps( INDEX ctFineMips, ULONG *pulMipmaps, PIX pixWidth, PIX pixHeight, INDEX iFilter=NONE);

// colorize mipmaps
extern void ColorizeMipmaps( INDEX i1stMipmapToColorize, ULONG *pulMipmaps, PIX pixWidth, PIX pixHeight);


// performs dithering of a 32-bit bipmap (can be in-place)
extern void DitherBitmap( INDEX iDitherType, ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight,
                          PIX pixCanvasWidth=0, PIX pixCanvasHeight=0);
// performs dithering of a 32-bit mipmaps (can be in-place)
extern void DitherMipmaps( INDEX iDitherType, ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight);

// adjust bitmap's saturation and/or hue (hue 0 and saturation 256 are defaults)
extern void AdjustBitmapColor( ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight, 
                               SLONG const slHueShift, SLONG const slSaturation);

// applies filter to 32-bit bitmap (iFilter from -6 to 6; negative=sharpen, positive=blur, 0=none)
extern void FilterBitmap( INDEX iFilter, ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight,
                          PIX pixCanvasWidth=0, PIX pixCanvasHeight=0);



// retrives number of mip-maps for the given bitmap size
__forceinline INDEX GetNoOfMipmaps( PIX pixWidth, PIX pixHeight) { return( FastLog2( Min(pixWidth, pixHeight))+1); }

// retrives memory offset of a specified mip-map or a size of all mip-maps (IN PIXELS!)
// (zero offset means first, i.e. largest mip-map)
extern PIX GetMipmapOffset( INDEX iMipLevel, PIX pixWidth, PIX pixHeight);

// return offset, pointer and dimensions of mipmap of specified size inside texture or shadowmap mipmaps
extern INDEX GetMipmapOfSize( PIX pixSize, ULONG *&pulFrame, PIX &pixProbeWidth, PIX &pixProbeHeight);


// flat white texture
ENGINE_API extern CTextureData *_ptdFlat;

// global texture parameters in engine
ENGINE_API extern CTexParams _tpGlobal[GFX_MAXTEXUNITS];

// pointer to global graphics library object
ENGINE_API extern CGfxLibrary *_pGfx;
// forced texture upload quality (0 = default, 16 = force 16-bit, 32 = force 32-bit)
ENGINE_API extern INDEX _iTexForcedQuality;


#endif  /* include-once check. */

