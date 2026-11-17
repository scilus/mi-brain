set(MITK_PLUGINS
	org.imeka.common.app:ON
	org.imeka.common.eager:ON
	org.imeka.common.view:ON
	org.imeka.property.eager:ON
	org.imeka.mibrain.app:ON
	org.imeka.mibrain.eager:ON
	org.imeka.mibrain.view:ON
	org.imeka.mibrain.rtt.view:ON
)

list(APPEND MITK_PLUGIN_REGEX_LIST "^org_imeka_[a-zA-Z0-9_]+$")