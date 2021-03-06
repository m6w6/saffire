/*
 Copyright (c) 2012-2013, The Saffire Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Saffire Group the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <string.h>
#include "general/output.h"
#include "modules/module_api.h"
#include "objects/object.h"
#include "objects/objects.h"
#include "general/dll.h"
#include "version.h"
#include "vm/vm.h"
#include "general/smm.h"

#define NO_FCGI_DEFINES
#include "fcgi_stdio.h"

FCGX_ParamArray fcgi_env;

/**
 *
 */
SAFFIRE_MODULE_METHOD(fastcgi, environment) {
    t_hash_table *ht = ht_create();

    if (fcgi_env != NULL) {
        char **p, *c;
        for (p = fcgi_env; *p != NULL; ++p) {
            c = strchr(*p, '=') + 1;
            char *k = smm_zalloc((c-*p));
            strncpy(k, *p, (c-*p)-1);
            ht_add(ht, k,  object_new(Object_String, 1, c));
            smm_free(k);
        }
    }

    printf("\n\n\n\n=============== ENV ============\n");
    t_hash_iter iter;
    ht_iter_init(&iter, ht);
    while (ht_iter_valid(&iter)) {
        char *val = ht_iter_value(&iter);
        char *key = ht_iter_key(&iter);

        printf("Key: '%-20s' => '%s'\n", key, val);

        ht_iter_next(&iter);
    }
    printf("================================\n\n\n");


    RETURN_HASH(ht);
}



t_object fastcgi_struct = { OBJECT_HEAD_INIT("fastcgi", objectTypeAny, OBJECT_TYPE_INSTANCE, NULL) };

static void _init(void) {
    fastcgi_struct.attributes = ht_create();

    object_add_internal_method((t_object *)&fastcgi_struct, "environment",  CALLABLE_FLAG_STATIC, ATTRIB_VISIBILITY_PUBLIC, module_fastcgi_method_environment);
}

static void _fini(void) {
    // Destroy methods and properties
    ht_destroy(fastcgi_struct.attributes);
}

static t_object *_objects[] = {
    &fastcgi_struct,
    NULL
};

t_module module_sapi_fastcgi = {
    "::_sfl::sapi::fastcgi",
    "FastCGI sapi module",
    _objects,
    _init,
    _fini
};
