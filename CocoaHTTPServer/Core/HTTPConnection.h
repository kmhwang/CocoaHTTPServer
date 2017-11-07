#import <Foundation/Foundation.h>

@class GCDAsyncSocket;
@class HTTPMessage;
@class HTTPServer;
@class WebSocket;
@protocol HTTPResponse;


#define HTTPConnectionDidDieNotification  @"HTTPConnectionDidDie"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface HTTPConfig : NSObject
{
    HTTPServer __unsafe_unretained *server;
    NSString __strong *documentRoot;
    dispatch_queue_t queue;
}

- (id)initWithServer:(HTTPServer *)server documentRoot:(NSString *)documentRoot;
- (id)initWithServer:(HTTPServer *)server documentRoot:(NSString *)documentRoot queue:(dispatch_queue_t)q;

@property (nonatomic, unsafe_unretained, readonly) HTTPServer *server;
@property (nonatomic, strong, readonly) NSString *documentRoot;
@property (nonatomic, readonly) dispatch_queue_t queue;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface HTTPConnection : NSObject

@property (nonatomic, readonly) dispatch_queue_t connectionQueue;
@property (nonatomic, readonly) GCDAsyncSocket *asyncSocket;
@property (nonatomic, readonly) HTTPConfig *config;

@property (nonatomic, readonly) BOOL started;

@property (nonatomic, readonly) HTTPMessage *request;
@property (nonatomic, readonly) unsigned int numHeaderLines;

@property (nonatomic, readonly) BOOL sentResponseHeaders;

@property (nonatomic, readonly) NSString *nonce;
@property (nonatomic, readonly) long lastNC;

@property (nonatomic, readonly) NSObject<HTTPResponse> *httpResponse;

@property (nonatomic, readonly) NSMutableArray *ranges;
@property (nonatomic, readonly) NSMutableArray *ranges_headers;
@property (nonatomic, readonly) NSString *ranges_boundry;
@property (nonatomic, readonly) int rangeIndex;

@property (nonatomic, readonly) UInt64 requestContentLength;
@property (nonatomic, readonly) UInt64 requestContentLengthReceived;
@property (nonatomic, readonly) UInt64 requestChunkSize;
@property (nonatomic, readonly) UInt64 requestChunkSizeReceived;

@property (nonatomic, readonly) NSMutableArray *responseDataSizes;

- (id)initWithAsyncSocket:(GCDAsyncSocket *)newSocket configuration:(HTTPConfig *)aConfig;

- (void)start;
- (void)stop;

- (void)startConnection;

- (BOOL)supportsMethod:(NSString *)method atPath:(NSString *)path;
- (BOOL)expectsRequestBodyFromMethod:(NSString *)method atPath:(NSString *)path;

- (BOOL)isSecureServer;
- (NSArray *)sslIdentityAndCertificates;

- (BOOL)isPasswordProtected:(NSString *)path;
- (BOOL)useDigestAccessAuthentication;
- (NSString *)realm;
- (NSString *)passwordForUser:(NSString *)username;

- (NSDictionary *)parseParams:(NSString *)query;
- (NSDictionary *)parseGetParams;

- (NSString *)requestURI;

- (NSArray *)directoryIndexFileNames;
- (NSString *)filePathForURI:(NSString *)path;
- (NSString *)filePathForURI:(NSString *)path allowDirectory:(BOOL)allowDirectory;
- (NSObject<HTTPResponse> *)httpResponseForMethod:(NSString *)method URI:(NSString *)path;
- (WebSocket *)webSocketForURI:(NSString *)path;

- (void)prepareForBodyWithSize:(UInt64)contentLength;
- (void)processBodyData:(NSData *)postDataChunk;
- (void)finishBody;

- (void)handleVersionNotSupported:(NSString *)version;
- (void)handleAuthenticationFailed;
- (void)handleResourceNotFound;
- (void)handleInvalidRequest:(NSData *)data;
- (void)handleUnknownMethod:(NSString *)method;

- (NSData *)preprocessResponse:(HTTPMessage *)response;
- (NSData *)preprocessErrorResponse:(HTTPMessage *)response;

- (void)finishResponse;

- (BOOL)shouldDie;
- (void)die;

@end

@interface HTTPConnection (AsynchronousHTTPResponse)
- (void)responseHasAvailableData:(NSObject<HTTPResponse> *)sender;
- (void)responseDidAbort:(NSObject<HTTPResponse> *)sender;
@end
