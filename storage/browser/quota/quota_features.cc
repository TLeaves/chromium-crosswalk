// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "storage/browser/quota/quota_features.h"

namespace storage {

namespace features {

const base::Feature kQuotaExpandPoolSize{"QuotaExpandPoolSize",
                                         base::FEATURE_DISABLED_BY_DEFAULT};

// modify the values referenced from chromium 115+
constexpr base::FeatureParam<double> kExperimentalPoolSizeRatio{
    &kQuotaExpandPoolSize, "PoolSizeRatio", 0.8}; /* 80% */

constexpr base::FeatureParam<double> kPerHostRatio{
    &kQuotaExpandPoolSize, "PerHostRatio", 0.75}; /* 75% */

// StaticHostQuota enables a simpler per-host quota model, where the quota is
// only based on disk capacity (partition size). When the flag is disabled, the
// quota computation takes into account free disk space, in addition to the
// disk's total capacity.
const base::Feature kStaticHostQuota{"StaticHostQuota",
                                     base::FEATURE_DISABLED_BY_DEFAULT};

// QuotaUnlimitedPoolSize removes limitations around disk space consumption with
// respect to client-side storage web platform APIs. When enabled, quota will
// set no limit on how much space a single origin can consume, as well as
// removing limits on how much disk space the temporary pool can consume.
const base::Feature kQuotaUnlimitedPoolSize{"QuotaUnlimitedPoolSize",
                                            base::FEATURE_DISABLED_BY_DEFAULT};
}  // namespace features
}  // namespace storage
