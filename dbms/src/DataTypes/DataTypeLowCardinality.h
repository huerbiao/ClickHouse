#pragma once
#include <DataTypes/IDataType.h>
#include <Columns/IColumnUnique.h>

namespace DB
{

class DataTypeLowCardinality : public IDataType
{
private:
    DataTypePtr dictionary_type;

public:
    DataTypeLowCardinality(DataTypePtr dictionary_type_);

    const DataTypePtr & getDictionaryType() const { return dictionary_type; }

    String doGetName() const override
    {
        return "LowCardinality(" + dictionary_type->getName() + ")";
    }
    const char * getFamilyName() const override { return "LowCardinality"; }
    TypeIndex getTypeId() const override { return TypeIndex::LowCardinality; }

    void enumerateStreams(const StreamCallback & callback, SubstreamPath & path) const override;

    void serializeBinaryBulkStatePrefix(
            SerializeBinaryBulkSettings & settings,
            SerializeBinaryBulkStatePtr & state) const override;

    void serializeBinaryBulkStateSuffix(
            SerializeBinaryBulkSettings & settings,
            SerializeBinaryBulkStatePtr & state) const override;

    void deserializeBinaryBulkStatePrefix(
            DeserializeBinaryBulkSettings & settings,
            DeserializeBinaryBulkStatePtr & state) const override;

    void serializeBinaryBulkWithMultipleStreams(
            const IColumn & column,
            size_t offset,
            UInt64 limit,
            SerializeBinaryBulkSettings & settings,
            SerializeBinaryBulkStatePtr & state) const override;

    void deserializeBinaryBulkWithMultipleStreams(
            IColumn & column,
            UInt64 limit,
            DeserializeBinaryBulkSettings & settings,
            DeserializeBinaryBulkStatePtr & state) const override;

    void serializeBinary(const Field & field, WriteBuffer & ostr) const override;
    void deserializeBinary(Field & field, ReadBuffer & istr) const override;

    void serializeBinary(const IColumn & column, size_t row_num, WriteBuffer & ostr) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeBinary, ostr);
    }
    void deserializeBinary(IColumn & column, ReadBuffer & istr) const override
    {
        deserializeImpl(column, &IDataType::deserializeBinary, istr);
    }

    void serializeTextEscaped(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeAsTextEscaped, ostr, settings);
    }

    void deserializeTextEscaped(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override
    {
        deserializeImpl(column, &IDataType::deserializeAsTextEscaped, istr, settings);
    }

    void serializeTextQuoted(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeAsTextQuoted, ostr, settings);
    }

    void deserializeTextQuoted(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override
    {
        deserializeImpl(column, &IDataType::deserializeAsTextQuoted, istr, settings);
    }

    void serializeTextCSV(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeAsTextCSV, ostr, settings);
    }

    void deserializeTextCSV(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override
    {
        deserializeImpl(column, &IDataType::deserializeAsTextCSV, istr, settings);
    }

    void serializeText(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeAsText, ostr, settings);
    }

    void serializeTextJSON(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeAsTextJSON, ostr, settings);
    }
    void deserializeTextJSON(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override
    {
        deserializeImpl(column, &IDataType::deserializeAsTextJSON, istr, settings);
    }

    void serializeTextXML(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeAsTextXML, ostr, settings);
    }

    void serializeProtobuf(const IColumn & column, size_t row_num, ProtobufWriter & protobuf) const override
    {
        serializeImpl(column, row_num, &IDataType::serializeProtobuf, protobuf);
    }

    MutableColumnPtr createColumn() const override;

    Field getDefault() const override { return dictionary_type->getDefault(); }

    bool equals(const IDataType & rhs) const override;

    bool isParametric() const override { return true; }
    bool haveSubtypes() const override { return true; }
    bool cannotBeStoredInTables() const override { return dictionary_type->cannotBeStoredInTables(); }
    bool shouldAlignRightInPrettyFormats() const override { return dictionary_type->shouldAlignRightInPrettyFormats(); }
    bool textCanContainOnlyValidUTF8() const override { return dictionary_type->textCanContainOnlyValidUTF8(); }
    bool isComparable() const override { return dictionary_type->isComparable(); }
    bool canBeComparedWithCollation() const override { return dictionary_type->canBeComparedWithCollation(); }
    bool canBeUsedAsVersion() const override { return dictionary_type->canBeUsedAsVersion(); }
    bool isSummable() const override { return dictionary_type->isSummable(); }
    bool canBeUsedInBitOperations() const override { return dictionary_type->canBeUsedInBitOperations(); }
    bool canBeUsedInBooleanContext() const override { return dictionary_type->canBeUsedInBooleanContext(); }
    bool isValueRepresentedByNumber() const override { return dictionary_type->isValueRepresentedByNumber(); }
    bool isValueRepresentedByInteger() const override { return dictionary_type->isValueRepresentedByInteger(); }
    bool isValueUnambiguouslyRepresentedInContiguousMemoryRegion() const override { return true; }
    bool haveMaximumSizeOfValue() const override { return dictionary_type->haveMaximumSizeOfValue(); }
    size_t getMaximumSizeOfValueInMemory() const override { return dictionary_type->getMaximumSizeOfValueInMemory(); }
    size_t getSizeOfValueInMemory() const override { return dictionary_type->getSizeOfValueInMemory(); }
    bool isCategorial() const override { return false; }
    bool isNullable() const override { return false; }
    bool onlyNull() const override { return false; }
    bool lowCardinality() const override { return true; }

    static MutableColumnUniquePtr createColumnUnique(const IDataType & keys_type);
    static MutableColumnUniquePtr createColumnUnique(const IDataType & keys_type, MutableColumnPtr && keys);

private:

    template <typename OutputStream, typename ... Args>
    using SerializeFunctionPtr = void (IDataType::*)(const IColumn &, size_t, OutputStream &, Args & ...) const;

    template <typename OutputStream, typename ... Args>
    void serializeImpl(const IColumn & column, size_t row_num, SerializeFunctionPtr<OutputStream, Args ...> func,
                       OutputStream & ostr, Args & ... args) const;

    template <typename ... Args>
    using DeserializeFunctionPtr = void (IDataType::*)(IColumn &, ReadBuffer &, Args & ...) const;

    template <typename ... Args>
    void deserializeImpl(IColumn & column, DeserializeFunctionPtr<Args ...> func,
                         ReadBuffer & istr, Args & ... args) const;

    template <typename Creator>
    static MutableColumnUniquePtr createColumnUniqueImpl(const IDataType & keys_type, const Creator & creator);
};

/// Returns dictionary type if type is DataTypeLowCardinality, type otherwise.
DataTypePtr removeLowCardinality(const DataTypePtr & type);

/// Remove LowCardinality recursively from all nested types.
DataTypePtr recursiveRemoveLowCardinality(const DataTypePtr & type);

/// Remove LowCardinality recursively from all nested columns.
ColumnPtr recursiveRemoveLowCardinality(const ColumnPtr & column);

/// Convert column of type from_type to type to_type by converting nested LowCardinality columns.
ColumnPtr recursiveLowCardinalityConversion(const ColumnPtr & column, const DataTypePtr & from_type, const DataTypePtr & to_type);

}
