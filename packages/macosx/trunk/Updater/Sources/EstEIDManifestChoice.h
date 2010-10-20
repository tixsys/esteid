/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestNode.h"

/**
 * The EstEIDManifestChoice represents a switch in the manifest. The first child that evaluates to YES is used.
 * 
 * <switch>
 *   <component arch="ppc64" ... />
 *   <component arch="ppc32" ... />
 *   <component ... />
 * </switch>
 */
@interface EstEIDManifestChoice : EstEIDManifestTreeNode

@end
