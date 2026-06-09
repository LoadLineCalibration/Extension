/*=============================================================================
	ExtensionPrivate_Reconstructed.h: compile-oriented reconstruction scaffold.
	Pass17/Pass27.

	Pass28 note: the compile-island mode was only a toolchain smoke test.
	For a useful DLL this header must export the real Extension classes and the
	project must compile the full implementation tree.  Do not ship a staged
	shell that merely links.
=============================================================================*/

#ifndef EXTENSION_PRIVATE_RECONSTRUCTED_H
#define EXTENSION_PRIVATE_RECONSTRUCTED_H

#ifndef EXTENSION_API
#define EXTENSION_API DLL_EXPORT
#endif

#include "Extension.h"

extern void ExtInitNames_Reconstructed();
extern void RegisterExtensionNatives_Reconstructed();

extern const TCHAR* XReconGetInterfaceConfigFilename();
extern UBOOL XReconShouldLogDiagnostics();
extern UBOOL XReconGetCompatibilityBool(const TCHAR* Key, UBOOL bDefault);

extern FTextureInfo* XReconGetGCTextureInfo(UCanvas* Canvas, UTexture* Texture);
extern void XReconCleanGCTextureInfoCache();

extern XWindow* XReconPushConfigAuthority(XWindow* NewAuthority);
extern void XReconPopConfigAuthority(XWindow* OldAuthority);

#endif
