#!/usr/bin/env python3
#
# Copyright (c) 2026 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import os
import re
import sys

EXCLUDED_METHOD_NAMES = {
    "New",
    "DownCast",
    "GetImplementation",
}

BASE_HANDLE_ROOT_NAMES = {
    "BaseHandle",
}

UNSUPPORTED_TYPE_KEYWORDS = (
    "std::",
    "Callback",
    "Functor",
    "Function",
    "Delegate",
    "Signal",
    "Bridge",
    "ConnectionTracker",
)

SUPPORTED_BUILTIN_TYPES = {
    "bool",
    "char",
    "short",
    "int",
    "long",
    "unsigned",
    "unsigned char",
    "unsigned short",
    "unsigned int",
    "unsigned long",
    "signed char",
    "signed short",
    "signed int",
    "signed long",
    "float",
    "double",
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "size_t",
    "Dali::String",
    "String",
    "StringView",
    "Dali::StringView",
    "Vector2",
    "Vector3",
    "Vector4",
    "Dali::Vector2",
    "Dali::Vector3",
    "Dali::Vector4",
    "Matrix",
    "Matrix3",
    "Quaternion",
    "Degree",
    "Radian",
    "Dali::Matrix",
    "Dali::Matrix3",
    "Dali::Quaternion",
    "Dali::Degree",
    "Dali::Radian",
    "Property::Index",
    "Property::Value",
    "Property::Map",
    "Property::Array",
    "Dali::Property::Index",
    "Dali::Property::Value",
    "Dali::Property::Map",
    "Dali::Property::Array",
    "AlphaFunction",
    "Dali::AlphaFunction",
}


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    text = re.sub(r"//.*", "", text)
    return text


def split_arguments(argument_text):
    argument_text = argument_text.strip()
    if not argument_text:
        return []

    arguments = []
    current = []
    depth = 0
    for character in argument_text:
        if character in "(<[{":
            depth += 1
        elif character in ")>]}":
            depth -= 1
        elif character == "," and depth == 0:
            arguments.append("".join(current).strip())
            current = []
            continue
        current.append(character)

    if current:
        arguments.append("".join(current).strip())

    return arguments


def split_default_value(argument):
    depth = 0
    for index, character in enumerate(argument):
        if character in "(<[{":
            depth += 1
        elif character in ")>]}":
            depth -= 1
        elif character == "=" and depth == 0:
            return argument[:index].strip(), argument[index + 1 :].strip()
    return argument.strip(), None


def parse_argument(argument):
    argument, default_value = split_default_value(argument)
    argument = re.sub(r"\bDALI_DEPRECATED_API\b", "", argument).strip()

    if not argument or argument == "void":
        return None

    match = re.match(r"^(.+[\s*&])([A-Za-z_]\w*)$", argument)
    argument_type = match.group(1).strip() if match else argument

    argument_type = argument_type.strip()

    return {
        "type": argument_type,
        "any_cast_type": get_any_cast_type(argument_type),
        "has_default": default_value is not None,
        "default": default_value,
    }


def get_any_cast_type(argument_type):
    any_cast_type = argument_type.strip()
    any_cast_type = re.sub(r"\s*&\s*$", "", any_cast_type).strip()
    any_cast_type = re.sub(r"^const\s+", "", any_cast_type).strip()
    any_cast_type = re.sub(r"^volatile\s+", "", any_cast_type).strip()
    return any_cast_type


def find_forward_declared_classes(text):
    return set(re.findall(r"\bclass\s+(?:\w+_API\s+)?([A-Za-z_]\w*)\s*;", text))


def get_base_type_name(type_name):
    base_type = get_any_cast_type(type_name)
    base_type = re.sub(r"\bconst\b", "", base_type)
    base_type = re.sub(r"\bvolatile\b", "", base_type)
    base_type = base_type.replace("&", "").replace("*", "").strip()
    base_type = base_type.split("<", 1)[0].strip()
    return base_type.split("::")[-1]


def get_base_type_candidates(type_name):
    base_type = get_type_without_cvref(type_name)
    simple_type = get_base_type_name(base_type)
    return {base_type, simple_type}


def get_type_without_cvref(type_name):
    base_type = type_name.strip()
    base_type = re.sub(r"\bconst\b", "", base_type)
    base_type = re.sub(r"\bvolatile\b", "", base_type)
    base_type = base_type.replace("&", "").strip()
    return re.sub(r"\s+", " ", base_type)


def get_nested_type_names(class_body):
    return set(re.findall(r"\benum(?:\s+class)?\s+([A-Za-z_]\w*)", class_body))


def qualify_nested_type(type_name, class_name, nested_type_names):
    base_type = get_type_without_cvref(type_name)
    if base_type in nested_type_names:
        return type_name.replace(base_type, f"{class_name}::{base_type}", 1)
    return type_name


def is_forward_declared_type(type_name, forward_declared_classes):
    return get_base_type_name(type_name) in forward_declared_classes


def get_required_argument_count(arguments):
    required_count = 0
    seen_default = False
    for argument in arguments:
        if argument["has_default"]:
            seen_default = True
        else:
            if seen_default:
                raise ValueError("Only trailing default parameters are supported.")
            required_count += 1
    return required_count


def normalize_type(type_name):
    normalized = type_name.strip()
    normalized = re.sub(r"\bDALI_DEPRECATED_API\b", "", normalized).strip()
    normalized = re.sub(r"\bconst\b", "", normalized).strip()
    normalized = re.sub(r"\bvolatile\b", "", normalized).strip()
    normalized = re.sub(r"\s+", " ", normalized)
    return normalized


def is_unsupported_type(type_name):
    normalized = normalize_type(type_name)
    if get_type_without_cvref(normalized) in {"AlphaFunction", "Dali::AlphaFunction"}:
        return False
    if "*" in normalized:
        return True
    if any(keyword in normalized for keyword in UNSUPPORTED_TYPE_KEYWORDS):
        return True
    if re.search(r"(^|[:\s])string(\s|$|&)", normalized):
        return True
    return False


def is_non_const_reference(argument_type):
    normalized = argument_type.strip()
    if "&" not in normalized:
        return False
    return not re.match(r"^const\b", normalized)


def is_volatile_reference(argument_type):
    return "&" in argument_type and re.search(r"\bvolatile\b", argument_type)


def is_any_supported_type(type_name, supported_base_handle_names=None, nested_type_names=None):
    supported_base_handle_names = supported_base_handle_names or set()
    nested_type_names = nested_type_names or set()
    base_type = get_type_without_cvref(type_name)

    if base_type in SUPPORTED_BUILTIN_TYPES or get_base_type_name(base_type) in SUPPORTED_BUILTIN_TYPES:
        return True
    if get_base_type_name(base_type) in supported_base_handle_names:
        return True
    if base_type in nested_type_names:
        return True
    if "::" in base_type and base_type.split("::")[-1] in nested_type_names:
        return True
    return False


def is_supported_argument(argument, forward_declared_classes=None, supported_base_handle_names=None, nested_type_names=None):
    forward_declared_classes = forward_declared_classes or set()
    argument_type = argument["type"]
    if is_unsupported_type(argument_type):
        return False
    if is_volatile_reference(argument_type):
        return False
    if is_non_const_reference(argument_type):
        return False
    if is_forward_declared_type(argument_type, forward_declared_classes) and not is_any_supported_type(argument["any_cast_type"], supported_base_handle_names, nested_type_names):
        return False
    if not is_any_supported_type(argument["any_cast_type"], supported_base_handle_names, nested_type_names):
        return False
    return True


def is_supported_return_type(return_type, forward_declared_classes=None, supported_base_handle_names=None, nested_type_names=None):
    forward_declared_classes = forward_declared_classes or set()
    return_type = return_type.strip()
    if return_type == "void":
        return True
    if is_forward_declared_type(return_type, forward_declared_classes):
        return False
    if is_unsupported_type(return_type):
        return False
    return is_any_supported_type(return_type, supported_base_handle_names, nested_type_names)


def find_matching_brace(text, open_brace_index):
    depth = 0
    for index in range(open_brace_index, len(text)):
        character = text[index]
        if character == "{":
            depth += 1
        elif character == "}":
            depth -= 1
            if depth == 0:
                return index
    raise ValueError("Class body has no matching closing brace.")


def find_namespace_at(text, position):
    namespace_stack = []
    pending_namespace = None
    depth = 0
    index = 0

    while index < position:
        if pending_namespace is None:
            namespace_match = re.match(r"\s*namespace\s+([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)\s*\{", text[index:])
            if namespace_match:
                pending_namespace = namespace_match.group(1)
                index += namespace_match.end() - 1
                continue

        character = text[index]
        if character == "{":
            depth += 1
            if pending_namespace:
                namespace_stack.append({"name": pending_namespace, "depth": depth})
                pending_namespace = None
        elif character == "}":
            depth -= 1
            while namespace_stack and namespace_stack[-1]["depth"] > depth:
                namespace_stack.pop()

        index += 1

    return "::".join(entry["name"] for entry in namespace_stack)


def parse_exported_class(text, class_name, export_api):
    class_pattern = re.compile(
        rf"\bclass\s+{re.escape(export_api)}\s+{re.escape(class_name)}\s*(?::(?P<bases>[^\{{]+))?\s*\{{",
        re.MULTILINE,
    )
    match = class_pattern.search(text)
    if not match:
        raise ValueError(f"Could not find class '{class_name}' marked with {export_api}.")

    open_brace_index = match.end() - 1
    close_brace_index = find_matching_brace(text, open_brace_index)
    return text[open_brace_index + 1 : close_brace_index]


def find_exported_class_declarations(text, export_api):
    declarations = []
    class_pattern = re.compile(
        rf"\bclass\s+{re.escape(export_api)}\s+(?P<name>[A-Za-z_]\w*)\s*(?::(?P<bases>[^\{{]+))?\s*\{{",
        re.MULTILINE,
    )

    for match in class_pattern.finditer(text):
        bases = match.group("bases") or ""
        base_names = {base.strip().split()[-1].split("::")[-1] for base in bases.split(",") if base.strip()}
        open_brace_index = match.end() - 1
        try:
            close_brace_index = find_matching_brace(text, open_brace_index)
        except ValueError:
            continue

        declarations.append(
            {
                "name": match.group("name"),
                "bases": base_names,
                "body": text[open_brace_index + 1 : close_brace_index],
                "namespace": find_namespace_at(text, match.start()),
            }
        )

    return declarations


def derives_from_base_handle(class_name, declarations_by_name, visiting=None):
    visiting = visiting or set()
    if class_name in BASE_HANDLE_ROOT_NAMES:
        return True
    if class_name in visiting:
        return False
    declaration = declarations_by_name.get(class_name)
    if not declaration:
        return False

    visiting.add(class_name)
    for base_name in declaration["bases"]:
        if base_name in BASE_HANDLE_ROOT_NAMES or derives_from_base_handle(base_name, declarations_by_name, visiting):
            return True
    return False


def has_declared_or_inherited_down_cast(class_name, declarations_by_name, visiting=None):
    visiting = visiting or set()
    if class_name in visiting:
        return False
    declaration = declarations_by_name.get(class_name)
    if not declaration:
        return False

    if "DownCast" in declaration["body"]:
        return True

    visiting.add(class_name)
    for base_name in declaration["bases"]:
        if has_declared_or_inherited_down_cast(base_name, declarations_by_name, visiting):
            return True
    return False


def is_invokable_handle_class(class_name, declarations_by_name):
    declaration = declarations_by_name.get(class_name)
    if not declaration:
        return False
    if not has_declared_or_inherited_down_cast(class_name, declarations_by_name):
        return False
    return derives_from_base_handle(class_name, declarations_by_name)


def split_public_top_level_statements(class_body):
    statements = []
    current = []
    access = "private"
    brace_depth = 0
    paren_depth = 0
    angle_depth = 0
    line_start = True

    index = 0
    while index < len(class_body):
        character = class_body[index]

        if brace_depth == 0 and paren_depth == 0 and angle_depth == 0:
            access_match = re.match(r"\s*(public|protected|private)\s*:", class_body[index:])
            if access_match:
                access = access_match.group(1)
                current = []
                index += access_match.end()
                line_start = True
                continue

        if character == "\n":
            line_start = True
        elif not character.isspace():
            line_start = False

        if character == "{" and paren_depth == 0:
            if access == "public" and brace_depth == 0:
                current = []
            brace_depth += 1
        elif character == "}" and paren_depth == 0 and brace_depth > 0:
            brace_depth -= 1
        elif brace_depth == 0:
            if character == "(":
                paren_depth += 1
            elif character == ")" and paren_depth > 0:
                paren_depth -= 1
            elif character == "<":
                angle_depth += 1
            elif character == ">" and angle_depth > 0:
                angle_depth -= 1

        if access == "public" and brace_depth == 0:
            current.append(character)
            if character == ";" and paren_depth == 0 and angle_depth == 0:
                statement = "".join(current).strip()
                if statement:
                    statements.append(statement)
                current = []
        elif brace_depth == 0 and access != "public":
            current = []

        index += 1

    return statements


def parse_member_statement(statement, class_name, forward_declared_classes=None, supported_base_handle_names=None, nested_type_names=None):
    forward_declared_classes = forward_declared_classes or set()
    supported_base_handle_names = supported_base_handle_names or set()
    nested_type_names = nested_type_names or set()
    statement = re.sub(r"\bDALI_DEPRECATED_API\b", "", statement).strip()
    statement = re.sub(r"\s+", " ", statement)

    if statement.startswith("template"):
        return None
    if "(" not in statement or ")" not in statement:
        return None
    if statement.startswith("using ") or statement.startswith("typedef "):
        return None
    if re.search(r"\b=\s*(delete|default)\s*;", statement):
        return None

    match = re.match(
        r"^(?P<prefix>.+?)\s+(?P<name>operator\s*[^\s(]+|[A-Za-z_]\w*)\s*"
        r"\((?P<arguments>.*)\)\s*(?P<suffix>[^;]*)\s*;$",
        statement,
    )
    if not match:
        return None

    name = match.group("name").replace(" ", "")
    prefix = match.group("prefix").strip()
    suffix = match.group("suffix").strip()

    if name == class_name or name == f"~{class_name}":
        return None
    if name.startswith("operator"):
        return None
    if name in EXCLUDED_METHOD_NAMES:
        return None
    if "static" in prefix.split():
        return None
    if "Signal" in prefix or name.endswith("Signal"):
        return None

    return_type = re.sub(r"\bvirtual\b", "", prefix)
    return_type = re.sub(r"\bexplicit\b", "", return_type)
    return_type = re.sub(r"\bstatic\b", "", return_type).strip()
    return_type = qualify_nested_type(return_type, class_name, nested_type_names)
    if not return_type:
        return None

    if "Signal" in return_type:
        return None
    if not is_supported_return_type(return_type, forward_declared_classes, supported_base_handle_names, nested_type_names):
        return None

    arguments = []
    for argument in split_arguments(match.group("arguments")):
        parsed_argument = parse_argument(argument)
        if parsed_argument:
            parsed_argument["type"] = qualify_nested_type(parsed_argument["type"], class_name, nested_type_names)
            parsed_argument["any_cast_type"] = qualify_nested_type(parsed_argument["any_cast_type"], class_name, nested_type_names)
            if not is_supported_argument(parsed_argument, forward_declared_classes, supported_base_handle_names, nested_type_names):
                return None
            arguments.append(parsed_argument)

    required_argument_count = get_required_argument_count(arguments)

    return {
        "return_type": " ".join(return_type.split()),
        "name": name,
        "arguments": arguments,
        "required_argument_count": required_argument_count,
    }


def parse_exported_class_methods(header_text, class_name, export_api):
    text = strip_comments(header_text)
    forward_declared_classes = find_forward_declared_classes(text)
    forward_declared_classes.discard(class_name)
    class_body = parse_exported_class(text, class_name, export_api)
    methods = []
    for statement in split_public_top_level_statements(class_body):
        method = parse_member_statement(statement, class_name, forward_declared_classes)
        if method:
            methods.append(method)
    return methods


def parse_exported_methods_from_body(header_text, class_name, class_body, supported_base_handle_names=None):
    text = strip_comments(header_text)
    forward_declared_classes = find_forward_declared_classes(text)
    forward_declared_classes.discard(class_name)
    nested_type_names = get_nested_type_names(class_body)
    methods = []
    for statement in split_public_top_level_statements(class_body):
        method = parse_member_statement(statement, class_name, forward_declared_classes, supported_base_handle_names, nested_type_names)
        if method:
            methods.append(method)
    return methods


def discover_header_files(scan_roots):
    header_files = []
    for scan_root in scan_roots:
        for root, _, files in os.walk(scan_root):
            for file_name in files:
                if file_name.endswith(".h"):
                    header_files.append(os.path.join(root, file_name))
    return sorted(header_files)


def make_include_path(header_path, scan_roots, include_prefixes):
    header_path = os.path.abspath(header_path)
    for scan_root, include_prefix in zip(scan_roots, include_prefixes):
        scan_root = os.path.abspath(scan_root)
        try:
            relative_path = os.path.relpath(header_path, scan_root)
        except ValueError:
            continue
        if not relative_path.startswith(".."):
            return os.path.join(include_prefix, relative_path).replace(os.sep, "/")
    raise ValueError(f"Could not derive include path for {header_path}.")


def discover_source_files(scan_roots):
    source_files = []
    for scan_root in scan_roots:
        if not scan_root:
            continue
        for root, _, files in os.walk(scan_root):
            for file_name in files:
                if file_name.endswith((".cpp", ".h", ".hpp")):
                    source_files.append(os.path.join(root, file_name))
    return sorted(source_files)


def discover_registered_type_names(scan_roots):
    registered_type_names = set()
    explicit_registered_type_names = set()

    for source_path in discover_source_files(scan_roots):
        try:
            with open(source_path, "r", encoding="utf-8") as source_file:
                source_text = strip_comments(source_file.read())
        except UnicodeDecodeError:
            continue

        for match in re.finditer(r"\bDALI_TYPE_REGISTRATION_BEGIN(?:_[A-Z_]+)?\s*\(\s*([^,\)]+)", source_text):
            registered_type = match.group(1).strip()
            registered_type_names.update(get_base_type_candidates(registered_type))

        for match in re.finditer(r"\bTypeRegistration\b[^\n;=]*\(\s*typeid\s*\(\s*([^\)]+)\s*\)", source_text, re.DOTALL):
            registered_type = match.group(1).strip()
            registered_type_names.update(get_base_type_candidates(registered_type))

        for match in re.finditer(r'\bTypeRegistration\b[^\n;=]*\(\s*"([^"]+)"', source_text, re.DOTALL):
            registered_type = match.group(1).strip()
            registered_type_names.add(registered_type)
            registered_type_names.add(registered_type.split("::")[-1])
            explicit_registered_type_names.add(registered_type)
            explicit_registered_type_names.add(registered_type.split("::")[-1])

    return registered_type_names, explicit_registered_type_names


def get_registered_type_name(class_name, registered_type_names, explicit_registered_type_names):
    if class_name in explicit_registered_type_names or class_name in registered_type_names:
        return class_name

    impl_type_name = f"{class_name}Impl"
    if impl_type_name in registered_type_names:
        return impl_type_name

    return None


def load_headers(scan_roots, include_prefixes, export_api, generate):
    headers = []
    for header_path in discover_header_files(scan_roots):
        with open(header_path, "r", encoding="utf-8") as header_file:
            header_text = header_file.read()
        stripped_text = strip_comments(header_text)
        declarations = find_exported_class_declarations(stripped_text, export_api)
        if declarations:
            headers.append(
                {
                    "path": header_path,
                    "include": make_include_path(header_path, scan_roots, include_prefixes),
                    "text": header_text,
                    "declarations": declarations,
                    "generate": generate,
                }
            )
    return headers


def discover_invokable_classes(args):
    scan_roots = args.scan_root or []
    include_prefixes = args.include_prefix or []
    if len(scan_roots) != len(include_prefixes):
        raise ValueError("--scan-root and --include-prefix must be provided the same number of times.")

    dependency_scan_roots = args.dependency_scan_root or []
    dependency_include_prefixes = args.dependency_include_prefix or []
    if len(dependency_scan_roots) != len(dependency_include_prefixes):
        raise ValueError("--dependency-scan-root and --dependency-include-prefix must be provided the same number of times.")

    headers = load_headers(scan_roots, include_prefixes, args.auto_export_api, True)
    headers.extend(load_headers(dependency_scan_roots, dependency_include_prefixes, args.auto_export_api, False))

    for export_api in args.dependency_export_api or []:
        headers.extend(load_headers(dependency_scan_roots, dependency_include_prefixes, export_api, False))

    declarations_by_name = {}
    for header in headers:
        for declaration in header["declarations"]:
            declarations_by_name[declaration["name"]] = declaration

    supported_base_handle_names = {
        class_name
        for class_name in declarations_by_name
        if derives_from_base_handle(class_name, declarations_by_name)
    }

    registered_type_names, explicit_registered_type_names = discover_registered_type_names(args.registration_scan_root or [])

    invokable_classes = []
    for header in headers:
        for declaration in header["declarations"]:
            class_name = declaration["name"]
            if class_name not in supported_base_handle_names:
                continue

            registered_type = get_registered_type_name(class_name, registered_type_names, explicit_registered_type_names)
            methods = []
            if header["generate"] and is_invokable_handle_class(class_name, declarations_by_name):
                if registered_type:
                    methods = parse_exported_methods_from_body(header["text"], class_name, declaration["body"], supported_base_handle_names)

            invokable_classes.append(
                {
                    "header": header["path"],
                    "include": header["include"],
                    "class_name": class_name,
                    "namespace": declaration["namespace"],
                    "registered_type": registered_type,
                    "methods": methods,
                }
            )

    return invokable_classes


def sanitize_symbol(value):
    return re.sub(r"\W+", "_", value).strip("_")


def generate_candidate(method, argument_count, indent):
    lines = []
    prefix = " " * indent
    cast_prefix = " " * (indent + 2)
    block_prefix = " " * (indent + 4)

    lines.append(f"{prefix}if(arguments.Count() == {argument_count}u)")
    lines.append(f"{prefix}{{")

    for index in range(argument_count):
        argument = method["arguments"][index]
        any_cast_type = argument["any_cast_type"]
        lines.append(f"{cast_prefix}const {any_cast_type}* argument{index} = arguments[{index}].IsType<{any_cast_type}>() ? Dali::AnyCast<{any_cast_type}>(&arguments[{index}]) : nullptr;")

    if argument_count:
        check = " && ".join(f"argument{index}" for index in range(argument_count))
        lines.append(f"{cast_prefix}if({check})")
        lines.append(f"{cast_prefix}{{")
        call_prefix = block_prefix
    else:
        call_prefix = cast_prefix

    call_arguments = ", ".join(f"*argument{index}" for index in range(argument_count))
    call = f"handle.{method['name']}({call_arguments})"
    if method["return_type"] == "void":
        lines.append(f"{call_prefix}{call};")
    else:
        lines.append(f"{call_prefix}auto methodResult = {call};")
        lines.append(f"{call_prefix}result = Dali::Any(methodResult);")
    lines.append(f"{call_prefix}return true;")

    if argument_count:
        lines.append(f"{cast_prefix}}}")

    lines.append(f"{prefix}}}")
    return lines


def get_candidate_signature(method, argument_count):
    return tuple(argument["any_cast_type"] for argument in method["arguments"][:argument_count])


def validate_method_group(method_name, methods):
    signatures = {}
    for method in methods:
        for argument_count in range(method["required_argument_count"], len(method["arguments"]) + 1):
            signature = get_candidate_signature(method, argument_count)
            previous = signatures.get((argument_count, signature))
            if previous:
                raise ValueError(
                    f"Ambiguous invokable overload for '{method_name}' with {argument_count} argument(s): "
                    f"{', '.join(signature) if signature else '<none>'}"
                )
            signatures[(argument_count, signature)] = method


def generate_method_group(namespace, class_name, registered_type, method_name, methods, qualified_handle=True):
    full_class_name = f"{namespace}::{class_name}" if namespace and qualified_handle else class_name
    symbol_class_name = sanitize_symbol(class_name)
    symbol_method_name = sanitize_symbol(method_name)
    function_name = f"Invoke_{symbol_class_name}_{symbol_method_name}"

    validate_method_group(method_name, methods)

    lines = []
    lines.append(
        f"bool {function_name}(Dali::BaseObject* object, const Dali::StringView& /*methodName*/, const Dali::InvokeArguments& arguments, Dali::InvokeResult& result)"
    )
    lines.append("{")
    lines.append("  result = Dali::Any();")
    lines.append("")
    lines.append(f"  {full_class_name} handle = {full_class_name}::DownCast(Dali::BaseHandle(object));")
    lines.append("  if(!handle)")
    lines.append("  {")
    lines.append("    return false;")
    lines.append("  }")
    lines.append("")
    for method in methods:
        for argument_count in range(method["required_argument_count"], len(method["arguments"]) + 1):
            lines.extend(generate_candidate(method, argument_count, 2))
            lines.append("")

    lines.append("  return false;")
    lines.append("}")
    lines.append("")
    lines.append(f'Dali::TypeMethod method_{symbol_class_name}_{symbol_method_name}("{registered_type}", "{method_name}", &{function_name});')
    return "\n".join(lines)


def group_methods_by_name(methods):
    method_groups = {}
    for method in methods:
        method_groups.setdefault(method["name"], []).append(method)
    return method_groups


def generate_class_methods(invokable_class, qualified_handle=True):
    output = []
    for method_name, method_group in group_methods_by_name(invokable_class["methods"]).items():
        try:
            output.append(
                generate_method_group(
                    invokable_class["namespace"],
                    invokable_class["class_name"],
                    invokable_class["registered_type"],
                    method_name,
                    method_group,
                    qualified_handle,
                )
            )
            output.append("")
        except ValueError as error:
            output.append(f"/* Skipped {invokable_class['class_name']}::{method_name}: {error} */")
            output.append("")
    return "\n".join(output)


def open_namespace(output, namespace):
    if not namespace:
        output.append("namespace")
        output.append("{")
        return

    for namespace_part in namespace.split("::"):
        output.append(f"namespace {namespace_part}")
        output.append("{")
    output.append("namespace")
    output.append("{")


def close_namespace(output, namespace):
    output.append("} // unnamed namespace")
    if namespace:
        for namespace_part in reversed(namespace.split("::")):
            output.append(f"}} // namespace {namespace_part}")


def generate(args, methods):
    output = []
    output.append("/*")
    output.append(f" * This file is generated by dali-invoke-method-generator.py from {args.header}.")
    output.append(" */")
    output.append("")
    output.append(f"#include <{args.include}>")
    output.append("#include <dali/devel-api/object/type-registry.h>")
    output.append("#include <dali/public-api/object/base-object.h>")
    output.append("#include <dali/public-api/object/invoke-method.h>")
    output.append("")
    open_namespace(output, args.namespace)
    output.append("")

    for method_name, method_group in group_methods_by_name(methods).items():
        output.append(generate_method_group(args.namespace, args.class_name, args.registered_type, method_name, method_group, False))
        output.append("")

    close_namespace(output, args.namespace)
    output.append("")
    return "\n".join(output)


def generate_discovered(invokable_classes):
    output = []
    output.append("/*")
    output.append(" * This file is generated by dali-invoke-method-generator.py from exported public API BaseHandle classes.")
    output.append(" */")
    output.append("")
    for include_path in sorted({invokable_class["include"] for invokable_class in invokable_classes}):
        output.append(f"#include <{include_path}>")
    output.append("#include <dali/devel-api/object/type-registry.h>")
    output.append("#include <dali/public-api/object/base-object.h>")
    output.append("#include <dali/public-api/object/invoke-method.h>")
    output.append("")
    classes_by_namespace = {}
    for invokable_class in invokable_classes:
        classes_by_namespace.setdefault(invokable_class["namespace"], []).append(invokable_class)

    for namespace in sorted(classes_by_namespace):
        open_namespace(output, namespace)
        output.append("")
        for invokable_class in classes_by_namespace[namespace]:
            output.append(generate_class_methods(invokable_class, False))
        close_namespace(output, namespace)
        output.append("")

    return "\n".join(output)


def main():
    parser = argparse.ArgumentParser(description="Generate DALi InvokeMethod wrappers from public API methods.")
    parser.add_argument("--header")
    parser.add_argument("--include")
    parser.add_argument("--namespace", default="")
    parser.add_argument("--class-name")
    parser.add_argument("--registered-type")
    parser.add_argument("--auto-export-api", required=True)
    parser.add_argument("--scan-root", action="append")
    parser.add_argument("--include-prefix", action="append")
    parser.add_argument("--dependency-scan-root", action="append")
    parser.add_argument("--dependency-include-prefix", action="append")
    parser.add_argument("--dependency-export-api", action="append")
    parser.add_argument("--registration-scan-root", action="append")
    parser.add_argument("--allow-type", action="append")
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    try:
        SUPPORTED_BUILTIN_TYPES.update(args.allow_type or [])
        SUPPORTED_BUILTIN_TYPES.update(get_base_type_name(allowed_type) for allowed_type in args.allow_type or [])
        if args.scan_root and not args.registration_scan_root:
            args.registration_scan_root = args.scan_root
        if args.scan_root:
            invokable_classes = discover_invokable_classes(args)
            generated_output = generate_discovered(invokable_classes)
        else:
            if not args.header or not args.include or not args.class_name or not args.registered_type:
                raise ValueError("--header, --include, --class-name, and --registered-type are required without --scan-root.")
            with open(args.header, "r", encoding="utf-8") as header_file:
                header_text = header_file.read()
            methods = parse_exported_class_methods(header_text, args.class_name, args.auto_export_api)
            generated_output = generate(args, methods)
    except ValueError as error:
        print(f"dali-invoke-method-generator: error: {error}", file=sys.stderr)
        return 1

    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    with open(args.output, "w", encoding="utf-8") as output_file:
        output_file.write(generated_output)

    return 0


if __name__ == "__main__":
    sys.exit(main())
