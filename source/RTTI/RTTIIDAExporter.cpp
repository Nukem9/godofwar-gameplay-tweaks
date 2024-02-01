/*

		RTTICSharpExporter exporter(RTTI::AllTypeDecls, RTTI::AllEnumDecls);
		exporter.ExportAll("C:\\gow_rtti_export\\");

		FILE *f = fopen("C:\\gow_rtti_export\\reflection.h", "w");

		if (f)
		{
			for (auto& handler : *GameModule::VMFunctions)
			{
				fprintf(f, "set_name(0x%p, \"GameModule::VMFunc_%s\");\n", asaddr(handler.m_Callback), handler.m_Name);
			}

			for (auto& handler : *GameModule::VMProperties)
			{
				fprintf(f, "set_name(0x%p, \"GameModule::VMProperty_%s\");\n", asaddr(handler.m_Callback), handler.m_Name);
			}

			for (auto& type : *RTTI::AllTypeDecls)
			{
				auto inherited = type.GetInheritedType();

				fprintf(f, "// Binary type = %d (0x%X)\n", type.GetBinaryTypeId(), type.GetBinaryTypeId());
				fprintf(f, "// sizeof() = 0x%llX\n", type.m_Size);
				fprintf(f, "// RTTI = 0x%p\n", asaddr(&type));
				fprintf(f, "class %s", type.m_Name);
				if (inherited)
					fprintf(f, " : public %s", inherited->m_Name);
				fprintf(f, "\n{\npublic:\n");

				auto members = type.CalculateMemberHeirarchy();

				for (auto member : members)
				{
					auto typeName = member->GetTypeString();
					auto parentType = member->GetParentType();

					fprintf(f, "\t%s %s;// 0x%X\n", typeName.c_str(), member->m_Name, member->m_Offset);
				}

				fprintf(f, "};\n\n");
			}

			for (auto& type : *RTTI::AllEnumDecls)
			{
				fprintf(f, "// Binary type = %d (0x%X)\n", type.GetBinaryTypeId(), type.GetBinaryTypeId());
				fprintf(f, "// RTTI = 0x%p\n", asaddr(&type));
				fprintf(f, "enum %s_ID%d\n{\n", type.m_Name, type.GetBinaryTypeId());

				for (uint32_t j = 0; j < type.m_EntryCount; j++)
					fprintf(f, "\t%s = %lld,\n", type.m_Entries[j], type.m_Values[j]);

				fprintf(f, "};\n\n");
			}

			fclose(f);
		}
	}

*/