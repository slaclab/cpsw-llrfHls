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

};


#endif  /* __LLRF_DESTNTRIG_H */
