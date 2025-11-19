#ifndef LOADCELL_H
#define LOADCELL_H

namespace LoadCell {
float adcToForce(float adcValue, size_t loadCellIndex);
} // namespace LoadCell

#endif // LOADCELL_H
