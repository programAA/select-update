#include "custom_table.h"
#include <cstring>

namespace bytedance_db_project {
CustomTable::CustomTable() {}

CustomTable::~CustomTable() {}

void CustomTable::Load(BaseDataLoader *loader) {
  // TODO: Implement this!
  rowTable.Load(loader);
  columnTable.Load(loader);
}

int32_t CustomTable::GetIntField(int32_t row_id, int32_t col_id) {
  // TODO: Implement this!
  return rowTable.GetIntField(row_id, col_id);
}

void CustomTable::PutIntField(int32_t row_id, int32_t col_id, int32_t field) {
  // TODO: Implement this!
  rowTable.PutIntField(row_id, col_id, field);
  columnTable.PutIntField(row_id, col_id, field);
}

int64_t CustomTable::ColumnSum() {
  // TODO: Implement this!
  return columnTable.ColumnSum();
}

int64_t CustomTable::PredicatedColumnSum(int32_t threshold1,
                                         int32_t threshold2) {
  // TODO: Implement this!
  return columnTable.PredicatedColumnSum(threshold1, threshold2);
}

int64_t CustomTable::PredicatedAllColumnsSum(int32_t threshold) {
  // TODO: Implement this!
  return rowTable.PredicatedAllColumnsSum(threshold);
}

int64_t CustomTable::PredicatedUpdate(int32_t threshold) {
  // TODO: Implement this!
  int64_t count = rowTable.PredicatedUpdate(threshold);
  columnTable.PredicatedUpdate(threshold);
  return count;
}
} // namespace bytedance_db_project