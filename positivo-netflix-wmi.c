#include <linux/module.h>	 
#include <linux/kernel.h>	 
#include <linux/init.h>	  
#include <linux/acpi.h>
#include <linux/string.h>

MODULE_LICENSE("GPL"); 

MODULE_AUTHOR("Edson Juliano Drosdeck"); 

MODULE_DESCRIPTION("Send netflix acpi key evento to acpi_listen"); 

MODULE_VERSION("1.0"); 


#define POSITIVO_WMI_EVENT_GUID "ABBC0F72-8EA1-11D1-00A0-C90629100000"
#define NETFLIX_KEY 28

static void wmi_hotkey_notify(u32 value, void *context)
    {
	struct acpi_buffer response = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *obj;
	acpi_status status;
	int code;

	status = wmi_get_event_data(value, &response);
	if (status != AE_OK) {
	    pr_err("bad event status 0x%x\n", status);
	    return;
	}

	obj = (union acpi_object *)response.pointer;

	if (obj && obj->type == ACPI_TYPE_INTEGER) {
		code = obj->integer.value;
                if ( code == NETFLIX_KEY ) acpi_bus_generate_netlink_event("Netflix", "positivo",0,0);
	}

	kfree(obj);
    }



static int __init wmi_hotkey_start(void) 
{
	int status;
        

        if (!wmi_has_guid(POSITIVO_WMI_EVENT_GUID)) {
	    printk("No known WMI GUID found\n");
	    return -ENODEV;
	}

	status = wmi_install_notify_handler(POSITIVO_WMI_EVENT_GUID,
					wmi_hotkey_notify, NULL);
		if (ACPI_FAILURE(status)) {
	    	
		return -ENODEV;
		}
	return 0; 
} 

static void __exit wmi_hotkey_end(void) 
{ 
	wmi_remove_notify_handler(POSITIVO_WMI_EVENT_GUID);
} 

module_init(wmi_hotkey_start); 
module_exit(wmi_hotkey_end); 

