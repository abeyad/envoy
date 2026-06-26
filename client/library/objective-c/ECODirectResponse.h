#pragma once

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, ECOMatchMode) {
  ECOMatchModeContains,
  ECOMatchModeExact,
  ECOMatchModePrefix,
  ECOMatchModeSuffix
};

@interface ECOHeaderMatcher : NSObject

@property (nonatomic, strong) NSString *name;
@property (nonatomic, strong) NSString *value;
@property (nonatomic) ECOMatchMode mode;

@end

@interface ECORouteMatcher : NSObject

@property (nonatomic, strong, nullable) NSString *fullPath;
@property (nonatomic, strong, nullable) NSString *pathPrefix;
@property (nonatomic, strong) NSArray<ECOHeaderMatcher *> *headers;

@end

@interface ECODirectResponse : NSObject

@property (nonatomic, strong) ECORouteMatcher *matcher;
@property (nonatomic) NSUInteger status;
@property (nonatomic, strong, nullable) NSString *body;
@property (nonatomic, strong) NSDictionary<NSString *, NSString *> *headers;

@end

NS_ASSUME_NONNULL_END
