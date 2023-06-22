// Desntination Trigger Path, hard coded...
// mmio/AppTop/AppCore/DestnTrig[4]
//

#ifndef __LLRF_DESTNTRIG_H
#define __LLRF_DESTNTRIG_H

#include <cpsw_api_user.h>
#include <cpsw_api_builder.h>

#define DESTNTRIG_PATH  "mmio/AppTop/AppCore/DestnTrig"
#define MAX_NUM_DEST    4


class IllrfDestnTrig;
typedef shared_ptr <IllrfDestnTrig> llrfDestnTrig;

class IllrfDestnTrig : public virtual IEntry {

public:
    static llrfDestnTrig create(Path p);

    virtual void setEnable(int index, uint32_t enable) = 0;
    virtual void setSource(int index, uint32_t source) = 0;
    virtual void setPolarity(int index, uint32_t polarity) = 0;
    virtual void setDelay(int index, uint32_t delay) = 0;
    virtual void setWidth(int index, uint32_t width) = 0;


};


#endif  /* __LLRF_DESTNTRIG_H */
