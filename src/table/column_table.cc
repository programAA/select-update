#include "column_table.h"
#include <cstring>
#include <iostream>

namespace bytedance_db_project {
ColumnTable::ColumnTable() {}

ColumnTable::~ColumnTable() {
  if (columns_ != nullptr) {
    delete columns_;
    columns_ = nullptr;
  }
}

//
// columnTable, which stores data in column-major format.
// That is, data is laid out like
//   col 1 | col 2 | ... | col m.
//
void ColumnTable::Load(BaseDataLoader *loader) {
  num_cols_ = loader->GetNumCols();
  std::vector<char *> rows = loader->GetRows();
  num_rows_ = rows.size();
  columns_ = new char[FIXED_FIELD_LEN * num_rows_ * num_cols_];

  for (int32_t row_id = 0; row_id < num_rows_; row_id++) {
    auto cur_row = rows.at(row_id);
    for (int32_t col_id = 0; col_id < num_cols_; col_id++) {
      int32_t offset = FIXED_FIELD_LEN * ((col_id * num_rows_) + row_id);
      std::memcpy(columns_ + offset, cur_row + FIXED_FIELD_LEN * col_id,
                  FIXED_FIELD_LEN);
    }
  }
}

int32_t ColumnTable::GetIntField(int32_t row_id, int32_t col_id) {
  // TODO: Implement this!
  int field;
  memcpy(&field, columns_ + FIXED_FIELD_LEN * (col_id * num_rows_ + row_id), FIXED_FIELD_LEN);
  return field;
}

void ColumnTable::PutIntField(int32_t row_id, int32_t col_id, int32_t field) {
  // TODO: Implement this!
  memcpy(columns_ + FIXED_FIELD_LEN * (col_id * num_rows_ + row_id), &field, FIXED_FIELD_LEN);
}

int64_t ColumnTable::ColumnSum() {
  // TODO: Implement this!
  int64_t sum = 0;
  int field;
  for(int32_t row_id=0; row_id<num_rows_; row_id++){
    memcpy(&field, columns_ + FIXED_FIELD_LEN * row_id, FIXED_FIELD_LEN);
    sum += field;
  }
  return sum;
}

int64_t ColumnTable::PredicatedColumnSum(int32_t threshold1, int32_t threshold2) {
  // TODO: Implement this!
  int64_t sum = 0;
  int32_t field;
  char *flag = new char[num_rows_]();
  // 依次访问1，2列， 先保存是否大于threshold的状态， 再访问0列， 根据状态执行加法
  for(int32_t row_id=0; row_id<num_rows_; row_id++){ // col1
    memcpy(&field, columns_ + FIXED_FIELD_LEN * (1 * num_rows_ + row_id), FIXED_FIELD_LEN);
    if(field > threshold1){
      flag[row_id] |= 1;
    }
  }
  for(int32_t row_id=0; row_id<num_rows_; row_id++){ // col2
    memcpy(&field, columns_ + FIXED_FIELD_LEN * (2 * num_rows_ + row_id), FIXED_FIELD_LEN);
    if(field < threshold2){
      flag[row_id] |= 2;
    }
  }
  for(int32_t row_id=0; row_id<num_rows_; row_id++){ // col0
    if((flag[row_id]>>1) & flag[row_id]){
      memcpy(&field, columns_ + FIXED_FIELD_LEN * row_id, FIXED_FIELD_LEN);
      sum += field;
    }
  }
  delete flag;
  return sum;
}

int64_t ColumnTable::PredicatedAllColumnsSum(int32_t threshold) {
  // TODO: Implement this!
  int64_t sum = 0;
  int32_t field;
  char *flag = new char[num_rows_]();
  // 先保存0列是否大于threshold的状态，再根据状态按列执行加法
  for(int32_t row_id=0; row_id<num_rows_; row_id++){ // col0
    memcpy(&field, columns_ + FIXED_FIELD_LEN * row_id, FIXED_FIELD_LEN);
    if(field > threshold){
      flag[row_id] = 1;
      sum += field;
    }
  }
  for(int32_t col_id=1; col_id<num_cols_; col_id++){
    for(int32_t row_id=0; row_id<num_rows_; row_id++){
      if(flag[row_id] == 1){
        memcpy(&field, columns_ + FIXED_FIELD_LEN * (col_id * num_rows_ + row_id), FIXED_FIELD_LEN);
        sum += field;
      }
    }
  }
  delete [] flag;
  return sum;
}

int64_t ColumnTable::PredicatedUpdate(int32_t threshold) {
  // TODO: Implement this!
  int64_t count = 0;
  int32_t field;
  int32_t* field3 = new int[num_rows_]();
  char* flag = new char[num_rows_]();
  for(int32_t row_id=0; row_id<num_rows_; row_id++){ // col0
    memcpy(&field, columns_ + FIXED_FIELD_LEN * row_id, FIXED_FIELD_LEN);
    if(field < threshold){
      flag[row_id] = 1;
      count++;
    }
  }
  for(int32_t row_id=0; row_id<num_rows_; row_id++){ // col2
    if(flag[row_id] == 1){
      memcpy(&field, columns_ + FIXED_FIELD_LEN * (2 * num_rows_ + row_id), FIXED_FIELD_LEN);
      field3[row_id] = field;
    }
  }
  for(int32_t row_id=0; row_id<num_rows_; row_id++){ // col3
    if(flag[row_id] == 1){
      memcpy(&field, columns_ + FIXED_FIELD_LEN * (3 * num_rows_ + row_id), FIXED_FIELD_LEN);
      field3[row_id] += field;
      memcpy(columns_ + FIXED_FIELD_LEN * (3 * num_rows_ + row_id), field3 + FIXED_FIELD_LEN * row_id, FIXED_FIELD_LEN);
    }
  }
  delete [] flag;
  delete [] field3;
  return count;
}
} // namespace bytedance_db_project