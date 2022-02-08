#include "row_table.h"
#include <cstring>

namespace bytedance_db_project {
RowTable::RowTable() {}

RowTable::~RowTable() {
  if (rows_ != nullptr) {
    delete rows_;
    rows_ = nullptr;
  }
}

void RowTable::Load(BaseDataLoader *loader) {
  num_cols_ = loader->GetNumCols();
  auto rows = loader->GetRows();
  num_rows_ = rows.size();
  rows_ = new char[FIXED_FIELD_LEN * num_rows_ * num_cols_];
  for (auto row_id = 0; row_id < num_rows_; row_id++) {
    auto cur_row = rows.at(row_id);
    std::memcpy(rows_ + row_id * (FIXED_FIELD_LEN * num_cols_), cur_row,
                FIXED_FIELD_LEN * num_cols_);
  }
}

int32_t RowTable::GetIntField(int32_t row_id, int32_t col_id) {
  // TODO: Implement this!
  int32_t field;
  std::memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + col_id), FIXED_FIELD_LEN);
  return field;
}

void RowTable::PutIntField(int32_t row_id, int32_t col_id, int32_t field) {
  // TODO: Implement this!
  std::memcpy(rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + col_id), &field, FIXED_FIELD_LEN); 
}

int64_t RowTable::ColumnSum() {
  // TODO: Implement this!
  int64_t sum = 0;
  int32_t field;
  for(int32_t row_id=0; row_id<num_rows_; row_id++){
    memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN);
    sum += field;
  }
  return sum;
}

int64_t RowTable::PredicatedColumnSum(int32_t threshold1, int32_t threshold2) {
  // TODO: Implement this!
  int64_t sum = 0;
  int32_t field;
  for(int32_t row_id=0; row_id<num_rows_; row_id++){
    memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 1), FIXED_FIELD_LEN);  // col1
    if(field > threshold1){
      memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 2), FIXED_FIELD_LEN); // col2
      if(field < threshold2){
        memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN); // col0
        sum += field;
      }
    }
  }
  return sum;
}

int64_t RowTable::PredicatedAllColumnsSum(int32_t threshold) {
  // TODO: Implement this!
  int64_t sum = 0;
  int32_t field;
  for(int32_t row_id=0; row_id<num_rows_; row_id++){
    memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN);    
    if(field > threshold){
      sum += field;
      for(int32_t col_id=1; col_id<num_cols_; col_id++){
        memcpy(&field, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + col_id), FIXED_FIELD_LEN);
        sum += field;
      }
    }
  }
  return sum;
}

int64_t RowTable::PredicatedUpdate(int32_t threshold) {
  // TODO: Implement this!
  int64_t count = 0;
  int32_t field0, field2, field3;
  for(int32_t row_id=0; row_id<num_rows_; row_id++){
    memcpy(&field0, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_), FIXED_FIELD_LEN);
    if(field0 < threshold){
      memcpy(&field2, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 2), FIXED_FIELD_LEN);
      memcpy(&field3, rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 3), FIXED_FIELD_LEN);
      field3 += field2;
      memcpy(rows_ + FIXED_FIELD_LEN * (row_id * num_cols_ + 3), &field3, FIXED_FIELD_LEN);
      count++;
    }
  }
  return count;
}
} // namespace bytedance_db_project