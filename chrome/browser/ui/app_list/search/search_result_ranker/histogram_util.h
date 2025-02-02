// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_APP_LIST_SEARCH_SEARCH_RESULT_RANKER_HISTOGRAM_UTIL_H_
#define CHROME_BROWSER_UI_APP_LIST_SEARCH_SEARCH_RESULT_RANKER_HISTOGRAM_UTIL_H_

#include <string>

namespace app_list {

// Represents situations that can occur during model configuration and
// initialization. These values persist to logs. Entries should not be
// renumbered and numeric values should never be reused.
enum class InitializationStatus {
  kInitialized = 0,
  kEphemeralUser = 1,
  kHashMismatch = 2,
  kInvalidConfigParameter = 3,
  kInvalidConfigPredictor = 4,
  kFakePredictorUsed = 5,
  kMaxValue = kFakePredictorUsed,
};

// Represents the status during a save or load of a model. These values persist
// to logs. Entries should not be renumbered and numeric values should never be
// reused.
enum class SerializationStatus {
  kSaveOk = 0,
  kLoadOk = 1,
  kModelReadError = 2,
  kModelWriteError = 3,
  kFromProtoError = 4,
  kToProtoError = 5,
  kPredictorMissingError = 6,
  kTargetsMissingError = 7,
  kConditionsMissingError = 8,
  kFakePredictorLoadingError = 9,
  kConditionalFrequencyPredictorLoadingError = 10,
  kFrecencyPredictorLoadingError = 11,
  kHourBinPredictorLoadingError = 12,
  kMarkovPredictorLoadingError = 13,
  kExponentialWeightsEnsembleLoadingError = 14,
  kMaxValue = kExponentialWeightsEnsembleLoadingError,
};

// Represents various uses of a RecurrenceRanker. These values persist to logs.
// Entries should not be renumbered and numeric values should never be reused.
enum class Usage {
  kRecord = 0,
  kRank = 1,
  kRemoveCondition = 2,
  kRenameCondition = 3,
  kRemoveTarget = 4,
  kRenameTarget = 5,
  kMaxValue = kRenameTarget,
};

// Represents whether an attempt to parse and convert a JSON configuration
// string into a RecurrenceRankerConfigProto succeeded or not. These values
// persist to logs. Entries should not be renumbered and numeric values should
// never be reused.
enum class JsonConfigConversionStatus {
  kFailure = 0,
  kSuccess = 1,
  kMaxValue = kSuccess,
};

void LogInitializationStatus(const std::string& suffix,
                             InitializationStatus status);

void LogSerializationStatus(const std::string& suffix,
                            SerializationStatus status);

void LogUsage(const std::string& suffix, Usage usage);

void LogJsonConfigConversionStatus(const std::string& suffix,
                                   JsonConfigConversionStatus status);

}  // namespace app_list

#endif  // CHROME_BROWSER_UI_APP_LIST_SEARCH_SEARCH_RESULT_RANKER_HISTOGRAM_UTIL_H_
