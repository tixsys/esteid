/* Copyright (c) 2009 Janek Priimann */

#import <CommonCrypto/CommonDigest.h>
#import <openssl/evp.h>
#import <openssl/bio.h>
#import <openssl/pem.h>
#import <openssl/rsa.h>
#import <openssl/sha.h>
#import "EstEIDConfiguration.h"
#import "EstEIDManifest.h"
#import "EstEIDManifestComponent.h"
#import "EstEIDManifestPackage.h"
#import "NSString+Additions.h"

NSString *EstEIDManifestErrorDomain = @"EstEIDManifestErrorDomain";

typedef struct _EstEIDManifestReaderNode {
	Class className;
	NSString *tag;
	NSDictionary *attributes;
	NSMutableString *contents;
	UInt32 count;
	struct _EstEIDManifestReaderNode *top, *first, *next;
} EstEIDManifestReaderNode;

static EstEIDManifestReaderNode *EstEIDManifestReaderNodeAllocate(EstEIDManifestReaderNode *top, Class className, NSString *tag, NSDictionary *attributes)
{
	EstEIDManifestReaderNode *node = malloc(sizeof(EstEIDManifestReaderNode));
	
	node->className = className;
	node->tag = [tag retain];
	node->attributes = [attributes retain];
	node->contents = nil;
	node->count = 0;
	node->top = top;
	node->first = NULL;
	node->next = NULL;
	
	return node;
}

static void EstEIDManifestReaderNodeDeallocate(EstEIDManifestReaderNode *node)
{
	while(node->first) {
		EstEIDManifestReaderNode *next = node->first->next;
		
		EstEIDManifestReaderNodeDeallocate(node->first);
		node->first = next;
	}
	
	[node->attributes release];
	[node->contents release];
	[node->tag release];
	free(node);
}

static NSArray *EstEIDManifestReaderNodeCopyChildren(EstEIDManifestReaderNode *node)
{
	UInt32 count = node->count;
	NSArray *nodes = nil;
	
	if(count > 0) {
		id *objects = malloc(sizeof(id) * count);
		UInt32 index = 0;
		
		node = node->first;
		
		while(node) {
			if(node->className) {
				nodes = EstEIDManifestReaderNodeCopyChildren(node);
				
				index += 1;
				objects[count - index] = [(EstEIDManifestNode *)[node->className alloc] initWithTag:node->tag attributes:node->attributes children:([nodes count] > 0) ? (id)nodes : (id)node->contents];
				[nodes release];
			}
			
			node = node->next;
		}
		
		if(index == count) {
			nodes = [[NSArray alloc] initWithObjects:objects count:count];
		} else {
			nodes = nil;
		}
		
		free(objects);
	}
	
	return nodes;
}

#pragma mark -

@interface EstEIDManifestReader : NSObject
{
	@private
	NSError *m_error;
	EstEIDManifestReaderNode *m_root;
	EstEIDManifestReaderNode *m_ptr;
}

- (id)initWithData:(NSData *)data;

- (NSError *)error;
- (NSArray *)nodes;

@end

@implementation EstEIDManifestReader

- (id)initWithData:(NSData *)data
{
	self = [super init];
	
	if(self) {
		NSXMLParser *parser = [[NSXMLParser alloc] initWithData:data];
		
		self->m_error = nil;
		self->m_root = EstEIDManifestReaderNodeAllocate(NULL, nil, nil, nil);
		self->m_ptr = self->m_root;
		
		[parser setDelegate:self];
		[parser parse];
		[parser release];
	}
	
	return self;
}

- (NSError *)error
{
	return self->m_error;
}

- (NSArray *)nodes
{
	return [EstEIDManifestReaderNodeCopyChildren(self->m_root) autorelease];
}

#pragma mark NSXMLParser+Delegate

- (void)parserDidStartDocument:(NSXMLParser *)parser
{
}

- (void)parserDidEndDocument:(NSXMLParser *)parser
{
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
	if(!self->m_ptr->contents) {
		self->m_ptr->contents = [[NSMutableString alloc] init];
	}
	
	[self->m_ptr->contents appendString:string];
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qualifiedName attributes:(NSDictionary *)attributeDict
{
	EstEIDManifestReaderNode *node = EstEIDManifestReaderNodeAllocate(self->m_ptr, (Class)[EstEIDManifestNode classForTag:elementName], elementName, attributeDict);
	
	if(!self->m_ptr->first) {
		self->m_ptr->first = node;
	} else {
		node->next = self->m_ptr->first;
		self->m_ptr->first = node;
	}
	
	if(node->className) {
		self->m_ptr->count += 1;
	}
	
	self->m_ptr = node;
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{
	if(self->m_ptr != self->m_root) {
		self->m_ptr = self->m_ptr->top;
	}
}

- (void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError
{
	[self->m_error release];
	self->m_error = [parseError retain];
}

#pragma mark NSObject

- (void)dealloc
{
	EstEIDManifestReaderNodeDeallocate(self->m_root);
	[self->m_error release];
	
	[super dealloc];
}

@end

#pragma mark -

@implementation EstEIDManifest

+ (id)manifestFromData:(NSData *)data
{
	EstEIDManifest *manifest = nil;
	
	if(data) {
		EstEIDManifestReader *reader = [[EstEIDManifestReader alloc] initWithData:data];
		NSArray *nodes = [reader nodes];
		
		if([nodes count] == 1) {
			EstEIDManifestNode *node = [nodes objectAtIndex:0];
			
			if([node isKindOfClass:[EstEIDManifest class]]) {
				((EstEIDManifest *)node)->m_data = [data retain];
				manifest = [[(EstEIDManifest *)node retain] autorelease];
			}
		}
		
		[reader release];
	}
	
	return manifest;
}

+ (id)manifestFromString:(NSString *)string
{
	return [self manifestFromData:[string dataUsingEncoding:NSUTF8StringEncoding]];
}

- (NSArray *)components
{
	NSEnumerator *enumerator = [[self nodes] objectEnumerator];
	NSMutableArray *components = [NSMutableArray array];
	EstEIDManifestNode *node;
	
	while((node = [enumerator nextObject]) != nil) {
		if([node test]) {
			[node writeComponentsToArray:components];
		}
	}
	
	return components;
}

- (NSData *)data
{
	return self->m_data;
}

- (NSString *)signature
{
	return self->m_signature;
}

- (NSString *)state
{
	NSMutableString *state = [NSMutableString string];
	NSArray *components = [self components];	
	int i, c;
	
	for(i = 0, c = [components count]; i < c; i++) {
		NSEnumerator *enumerator = [[(EstEIDManifestComponent *)[components objectAtIndex:i] packages] objectEnumerator];
		EstEIDManifestPackage *package;
		
		while((package = [enumerator nextObject]) != nil) {
			[state appendFormat:@"%@%@", [package identifier], [package version]];
		}
	}
	
	return [state sha1String];
}

- (BOOL)verifySignature:(NSData *)signature usingPublicKey:(NSString *)publicKey
{
	BOOL result = NO;
	
	if([signature length] > 0 && [self->m_data length] > 0 && publicKey) {
		char *ckey = (char *)[publicKey UTF8String];
		EVP_PKEY *pkey = NULL;
		BIO *bio;
		
		if((bio = BIO_new_mem_buf(ckey, (int)strlen(ckey))) != NULL) {
			DSA *dsa = 0;
			
			if(PEM_read_bio_DSA_PUBKEY(bio, &dsa, NULL, NULL) && (pkey = EVP_PKEY_new()) != NULL && EVP_PKEY_assign_DSA(pkey, dsa) != 1) {
				DSA_free(dsa);
				EVP_PKEY_free(pkey);
				pkey = NULL;
			}
			
			BIO_free(bio);
		}
		
		if(pkey) {
			UInt8 bytes[SHA_DIGEST_LENGTH];
			CC_SHA1_CTX sha1;
			EVP_MD_CTX ctx;
			
			CC_SHA1_Init(&sha1);
			CC_SHA1_Update(&sha1, [self->m_data bytes], [self->m_data length]);
			CC_SHA1_Final(&(bytes[0]), &sha1);
			
			if(EVP_VerifyInit(&ctx, EVP_dss1()) == 1) {
				EVP_VerifyUpdate(&ctx, &(bytes[0]), SHA_DIGEST_LENGTH);
				result = (EVP_VerifyFinal(&ctx, (unsigned char *)[signature bytes], [signature length], pkey) == 1) ? YES : NO;
			}
			
			EVP_PKEY_free(pkey);
		}
	}
	
	return result;
}

#pragma mark EstEIDManifestNode

+ (NSString *)tag
{
	return @"manifest";
}

- (id)initWithTag:(NSString *)tag attributes:(NSDictionary *)attributes children:(id)children
{
	self = [super initWithTag:tag attributes:attributes children:children];
	
	if(self) {
		self->m_signature = [[attributes objectForKey:@"signature"] init];
	}
	
	return self;
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_data release];
	[self->m_signature release];
	
	[super dealloc];
}

@end
