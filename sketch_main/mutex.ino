void lock(SemaphoreHandle_t l) {
  while (xSemaphoreTake(l, (TickType_t) 10) != pdTRUE) {}
  return;
}

void unlock(SemaphoreHandle_t l) {
  xSemaphoreGive(l);
}
