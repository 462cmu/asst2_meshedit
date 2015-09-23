#include "collada.h"

#include <assert.h>
#include <map>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#define PI 3.14159265

#define deg2rad(degree) degree / 180.0 * PI
#define rad2deg(radian) radian / PI * 180.0

#define stat(s) // cerr <<"[COLLADA Parser] "<< s << endl;

using namespace std;

namespace CMU462 {

// Parser //

XMLElement* ColladaParser::e_scenes;       // COLLADA library: scenes
XMLElement* ColladaParser::e_cameras;      // COLLADA library: cameras
XMLElement* ColladaParser::e_lights;       // COLLADA library: lights
XMLElement* ColladaParser::e_geometries;   // COLLADA library: geometries
XMLElement* ColladaParser::e_materials;    // COLLADA library: materials
XMLElement* ColladaParser::e_effects;      // COLLADA library: effects

XMLElement* find_instance( XMLElement* entry, string id ) {

  assert( entry );

  XMLElement* e_instance = entry->FirstChildElement();
  while( e_instance ) {
    if ( e_instance->Attribute("id") == id ) return e_instance;
    e_instance = e_instance->NextSiblingElement();
  }

  return NULL;
}

XMLElement* find_source( XMLElement* entry, string id ) {

  assert( entry );

  XMLElement* e_source = entry->FirstChildElement("source");
  while( e_source ) {
    if ( e_source->Attribute("id") == id ) return e_source;
    e_source = e_source->NextSiblingElement("source");
  }

  return NULL;
}

Color rgb_from_string ( string color_string ) {

  Color c;

  stringstream ss (color_string);
  ss >> c.r;
  ss >> c.g;
  ss >> c.b;
  c.a = 1.0;

  return c;

}

Color rgba_from_string ( string color_string ) {

  Color c;

  stringstream ss (color_string);
  ss >> c.r;
  ss >> c.g;
  ss >> c.b;
  ss >> c.a;

  return c;

}

int ColladaParser::load( const char* filename, Scene* scene ) {

  ifstream in( filename );
  if ( !in.is_open() ) {
    return -1;
  } in.close();

  XMLDocument doc;
  doc.LoadFile( filename );
  if ( doc.Error() ) {
    doc.PrintError();
    exit( 1 );
  }

  XMLElement* root = doc.FirstChildElement("COLLADA");
  if ( !root ) {
    stat("Error: not a COLLADA file!")
    exit( 1 );
  } else {
    stat("Loading COLLADA file...");
  }

  // save COLLADA library entry points
  e_scenes     = root->FirstChildElement("library_visual_scenes");
  e_cameras    = root->FirstChildElement("library_cameras"      );
  e_lights     = root->FirstChildElement("library_lights"       );
  e_geometries = root->FirstChildElement("library_geometries"   );
  e_materials  = root->FirstChildElement("library_materials"    );
  e_effects    = root->FirstChildElement("library_effects"      );

  // load assets
  XMLElement* e_asset = root->FirstChildElement("asset");
  if ( e_asset ) {

    // NOTE (sky):
    // We probably should take care of different Y directions here if we want
    // to support exports from different packages.
    XMLElement* up_axis = e_asset->FirstChildElement("up_axis");
    stat("Y direction: "<< up_axis->GetText());
  }

  // load scenes -
  XMLElement* e_scene = root->FirstChildElement("scene");
  if ( e_scene ) {

    XMLElement* e_instance = e_scene->FirstChildElement("instance_visual_scene");

    // NOTE (sky):
    // COLLADA seems to support multiple scenes in one file
    // right now the parser only loads the first scene

    // load all scenes
    // while (e_instance) {
    //   // load a single scene
    //   e_instance = e_scene->NextSiblingElement("instance_visual_scene");
    // }

    // load first scene
    if (e_instance) {

      const char* instance_url = e_instance->Attribute("url");
      if ( instance_url ) {

        // look for scene in scene library
        string instance_id = instance_url + 1;
        XMLElement* e_instance = find_instance( e_scenes, instance_id );
        if ( e_instance ) {
          stat("Loading scene: " << instance_id);
          parseScene(e_instance, *scene);
        } else {
          stat("Error: undefined scene instance: " << instance_id);
          return -1;
        }

      } else {

        // Note (Sky):
        // not sure if there are non-indirection encodings but
        // if there is then it should be handled here
        return 0;

      }
    }
  }

  return 0;

}

int ColladaParser::save( const char* filename, const Scene* scene ) {

  // TODO: not yet supported
  return 0;

}

void ColladaParser::parseScene( XMLElement* xml, Scene& scene ) {

  // parse all nodes
  XMLElement* e_node = xml->FirstChildElement("node");
  while ( e_node ){
    Node node = Node();
    parseNode(e_node, node);
    scene.nodes.push_back(node);
    e_node = e_node->NextSiblingElement("node");
  }

}

void ColladaParser::parseNode( XMLElement* xml, Node& node ) {

  node.id   = xml->Attribute( "id" );
  node.name = xml->Attribute("name");
  stat(" |- Node: " << node.name << " (id:" << node.id << ")");

  // parse transformation
  XMLElement* e_mat = xml->FirstChildElement("matrix");
  if ( e_mat ) { // given as matrix

    string s = e_mat->GetText();
    stringstream ss (s);

    Matrix4x4 mat; // collada uses row-majored representation
    ss >> mat(0,0); ss >> mat(0,1); ss >> mat(0,2); ss >> mat(0,3);
    ss >> mat(1,0); ss >> mat(1,1); ss >> mat(1,2); ss >> mat(1,3);
    ss >> mat(2,0); ss >> mat(2,1); ss >> mat(2,2); ss >> mat(2,3);
    ss >> mat(3,0); ss >> mat(3,1); ss >> mat(3,2); ss >> mat(3,3);

    node.transform = mat;

  } else { // given as transformation list

    // rotate
    Matrix4x4 R = Matrix4x4::identity();
    XMLElement* e_rotate = xml->FirstChildElement("rotate");
    while ( e_rotate ) {

      Matrix4x4 r = Matrix4x4::identity();

      string s = e_rotate->GetText();
      stringstream ss (s);

      string sid = e_rotate->Attribute("sid");
      switch ( sid.back() ) {
        case 'X':
          ss >> r(1,1); ss >> r(1,2);
          ss >> r(2,1); ss >> r(2,2);
          break;
        case 'Y':
          ss >> r(0,0); ss >> r(2,0);
          ss >> r(0,2); ss >> r(2,2);
          break;
        case 'Z':
          ss >> r(0,0); ss >> r(0,1);
          ss >> r(1,0); ss >> r(1,1);
          break;
        default:
          break;
      }

      R = r * R;

      e_rotate = e_rotate->NextSiblingElement("rotate");
    }

    // translate
    Matrix4x4 T = Matrix4x4::identity();
    XMLElement* e_translate = xml->FirstChildElement("translate");
    if ( e_translate ) {

      string s = e_translate->GetText();
      stringstream ss (s);

      ss >> T(0,3); ss >> T(1,3); ss >> T(2,3);

    }

    // scale
    Matrix4x4 S = Matrix4x4::identity();
    XMLElement* e_scale = xml->FirstChildElement("scale");
    while ( e_scale ) {

      string s = e_translate->GetText();
      stringstream ss (s);

      ss >> S(0,0); ss >> S(1,1); ss >> S(1,1);
    }

    // skew -
    // Note (sky): ignored for now

    // lookat -
    // Note (sky): ignored for now

    node.transform = T * R * S;
  }

  // camera instance
  XMLElement* e_instance_camera = xml->FirstChildElement("instance_camera");
  if ( e_instance_camera ) {
    const char* instance_url = e_instance_camera->Attribute("url");
    if ( instance_url ) {
      string camera_id = instance_url + 1;
      XMLElement* e_camera = find_instance( e_cameras, camera_id );
      if ( e_camera ) {
        Camera* camera = new Camera();
        parseCamera( e_camera, *camera );
        node.instance = camera;
      } else {
        stat("Error: undefined camera instance: "<< camera_id);
        exit( -1 );
      }
    } else {
      // handle non-indirection
    }
  }

  // light instance
  XMLElement* e_instance_light = xml->FirstChildElement("instance_light");
  if ( e_instance_light ) {
    const char* instance_url = e_instance_light->Attribute("url");
    if ( instance_url ) {
      string light_id = instance_url + 1;
      XMLElement* e_light = find_instance( e_lights, light_id );
      if ( e_light ) {
        Light* light = new Light();
        parseLight( e_light, *light );
        node.instance = light;
      } else {
        stat("Error: undefined light instance: "<< light_id);
        exit( -1 );
      }
    } else {
      // handle non-indirection
    }
  }

  // geometry instance
  XMLElement* e_instance_geometry = xml->FirstChildElement("instance_geometry");
  if ( e_instance_geometry ) {

    const char* instance_url = e_instance_geometry->Attribute("url");
    if ( instance_url ) {
      string geometry_id = instance_url + 1;
      XMLElement* e_geometry = find_instance( e_geometries, geometry_id );
      if ( e_geometry ) {
        Polymesh* polymesh = new Polymesh();
        parsePolymesh( e_geometry, *polymesh );
        node.instance = polymesh;
      } else {
        stat("Error: undefined geometry instance: "<< geometry_id);
        exit( -1 );
      }
    } else {
      // handle non-indirection
    }
  }

}

void ColladaParser::parseCamera( XMLElement* xml, Camera& camera ) {

  // name & id
  camera.id   = xml->Attribute(  "id"  );
  camera.name = xml->Attribute( "name" );
  camera.type = CAMERA;

  // optics
  XMLElement* e_optics = xml->FirstChildElement( "optics" );
  if ( !e_optics ) {
    stat("Error: optics not defined in camera: " << camera.id );
    exit( -1 );
  }

  // common
  XMLElement* e_common = e_optics->FirstChildElement( "technique_common" );
  if ( e_common ) {
    // perspective
    XMLElement* e_perspective = e_common->FirstChildElement( "perspective" );
    if ( e_perspective ) {

      XMLElement* e_xfov  = e_perspective->FirstChildElement( "xfov"  );
      XMLElement* e_yfov  = e_perspective->FirstChildElement( "yfov"  );
      XMLElement* e_znear = e_perspective->FirstChildElement( "znear" );
      XMLElement* e_zfar  = e_perspective->FirstChildElement( "zfar"  );

      camera.hfov  = e_xfov  ? atof(e_xfov  -> GetText()) : 0.0f;
      camera.vfov  = e_yfov  ? atof(e_yfov  -> GetText()) : 0.0f;
      camera.nclip = e_znear ? atof(e_znear -> GetText()) : 0.0f;
      camera.fclip = e_zfar  ? atof(e_zfar  -> GetText()) : 0.0f;

      // if vfov is not given, compute from aspect ratio
      if ( !e_yfov ) {
        XMLElement* e_aspect_ratio = e_perspective->FirstChildElement( "aspect_ratio" );
        float aspect_ratio = atof(e_aspect_ratio->GetText());
        camera.vfov = 2 * rad2deg(atan(tan(deg2rad(0.5 * camera.hfov)) / aspect_ratio));
      }
    }
  }

  // Note (sky):
  // ignoring extras

  // print summary
  stat("  |- " << camera);
}

void ColladaParser::parseLight( XMLElement* xml, Light& light ) {

  // name & id
  light.id   = xml->Attribute(  "id"  );
  light.name = xml->Attribute( "name" );
  light.type = LIGHT;

  // common
  XMLElement* e_common = xml->FirstChildElement( "technique_common" );
  if ( e_common ) {

    // ambient light
    XMLElement* e_ambient = e_common->FirstChildElement( "ambient" );
    if ( e_ambient ) {
      light.light_type = AMBIENT;
      XMLElement* e_color = e_ambient->FirstChildElement( "color" );
      if ( e_color ) {
        string color_string = e_color->GetText();
        light.color = rgb_from_string( color_string );
        light.attenuation = 1;
      }
    }

    // point light
    XMLElement* e_point = e_common->FirstChildElement( "point" );
    if ( e_point ) {

      light.light_type = POINT;

      // color
      XMLElement* e_color = e_point->FirstChildElement( "color" );
      if ( e_color ) {
        string color_string = e_color->GetText();
        light.color = rgb_from_string( color_string );
      }

      // attenuation
      XMLElement* e_attenuation = e_point->FirstChildElement( "constant_attenuation" );
      if ( e_attenuation ) {
        light.attenuation = atof(e_attenuation->GetText());
      }
    }

    // directional light
    XMLElement* e_directional = e_common->FirstChildElement( "directional" );
    if ( e_directional ) {

      light.light_type = DIRECTIONAL;

      // color
      XMLElement* e_color = e_directional->FirstChildElement( "color" );
      if ( e_color ) {
        string color_string = e_color->GetText();
        light.color = rgb_from_string( color_string );
      }

      // Note (sky):
      // direction?
    }

  }

  // Note (sky):
  // ignoring extras

  // print summary
  stat("  |- " << light);
}

void ColladaParser::parsePolymesh( XMLElement* xml, Polymesh& polymesh ) {

  // name & id
  polymesh.id   = xml->Attribute(  "id" );
  polymesh.name = xml->Attribute( "name" );
  polymesh.type = POLYMESH;


  XMLElement* e_mesh = xml->FirstChildElement( "mesh" );
  if ( !e_mesh ) {

    stat("Error: no mesh data defined in geometry: " << polymesh.id);
    exit( -1 );

  } else {

    // parse sources
    map< string, vector<float> > sources;
    XMLElement* e_source = e_mesh->FirstChildElement( "source" );
    while (e_source) {

      // source id
      string source_id = e_source->Attribute( "id" );

      // source float array - other formats not handled
      XMLElement* e_float_array = e_source->FirstChildElement( "float_array" );
      if ( e_float_array ) {

        float f; vector<float> floats;

        // load float array string
        string s = e_float_array->GetText();
        stringstream ss (s);

        // load float array
        size_t num_floats = e_float_array->IntAttribute( "count" );
        for (size_t i = 0; i < num_floats; ++i) {
          ss >> f; floats.push_back(f);
        }

        // add to sources
        sources[source_id] = floats;
      }

      // parse next source
      e_source = e_source->NextSiblingElement( "source" );
    }

    // parse vertices
    vector<Vector3D> vertices; string vertices_id;
    XMLElement* e_vertices = e_mesh->FirstChildElement( "vertices" );
    if ( e_vertices ) {

      vertices_id = e_vertices->Attribute( "id" );

      XMLElement* e_input = e_vertices->FirstChildElement( "input" );
      if ( e_input ) {

        // assert semantics
        string semantic = e_input->Attribute( "semantic" );
        assert ( semantic == "POSITION" );

        // construct from positions
        string source = e_input->Attribute( "source" ) + 1;
        if ( sources.find(source) != sources.end() ) {
          vector<float>& floats = sources[source];
          size_t num_floats = floats.size();
          for (size_t i = 0; i < num_floats; i += 3) {
            Vector3D v = Vector3D(floats[i], floats[i+1], floats[i+2]);
            vertices.push_back(v);
          }
        } else {
          stat("Error: undefined input source: " << source);
          exit( -1 );
        }
      } else {
        stat("Error: no input for vertices in geometry: " << polymesh.id);
        exit( -1 );
      }
    } else {
      stat("Error: no vertices defined in geometry: " << polymesh.id);
      exit( -1 );
    }

    // polylist
    XMLElement* e_polylist = e_mesh->FirstChildElement( "polylist" );
    if ( e_polylist ) {

      // material
      string material_id = e_polylist->Attribute( "material" );
      XMLElement* e_material = find_instance( e_materials, material_id );
      if ( e_material ) {
        Material* material = new Material();
        parseMaterial( e_material, *material );
        polymesh.material = material;
      } else {
        stat("Error: undefined reference to material: " << material_id);
        exit( -1 );
      }

      // input arrays & array offsets
      bool has_vertex_array   = false; size_t vertex_offset   = 0;
      bool has_normal_array   = false; size_t normal_offset   = 0;
      bool has_texcoord_array = false; size_t texcoord_offset = 0;

      // input sources
      XMLElement* e_input = e_polylist->FirstChildElement( "input" );
      while ( e_input ) {

        string semantic = e_input->Attribute( "semantic" );
        string source   = e_input->Attribute( "source" ) + 1;
        size_t offset   = e_input->IntAttribute( "offset" );

        // vertex array source
        if ( semantic == "VERTEX" ) {

          has_vertex_array = true;
          vertex_offset = offset;

          if ( source == vertices_id ) {
            polymesh.vertices.resize(vertices.size());
            copy(vertices.begin(), vertices.end(), polymesh.vertices.begin());
          } else {
            stat("Error: undefined source for VERTEX semantic: " << source);
            exit( -1 );
          }

        }

        // normal array source
        if ( semantic == "NORMAL" ) {

          has_normal_array = true;
          normal_offset = offset;

          if ( sources.find(source) != sources.end() ) {
            vector<float>& floats = sources[source];
            size_t num_floats = floats.size();
            for (size_t i = 0; i < num_floats; i += 3) {
              Vector3D n = Vector3D(floats[i], floats[i+1], floats[i+2]);
              polymesh.normals.push_back(n);
            }
          } else {
            stat("Error: undefined source for NORMAL semantic: " << source);
            exit( -1 );
          }

        }

        // texcoord array source
        if ( semantic == "TEXCOORD" ) {

          has_texcoord_array = true;
          texcoord_offset = offset;

          if ( sources.find(source) != sources.end() ) {
            vector<float>& floats = sources[source];
            size_t num_floats = floats.size();
            for (size_t i = 0; i < num_floats; i += 2) {
              Vector2D n = Vector2D(floats[i], floats[i+1]);
              polymesh.texcoords.push_back(n);
            }
          } else {
            stat("Error: undefined source for TEXCOORD semantic: " << source);
            exit( -1 );
          }

        }

        e_input = e_input->NextSiblingElement( "input" );
      }

      // polygon info
      size_t num_polygons = e_polylist->IntAttribute( "count" );
      size_t stride = ( has_vertex_array   ? 1 : 0 ) +
                      ( has_normal_array   ? 1 : 0 ) +
                      ( has_texcoord_array ? 1 : 0 ) ;

      // create polygon size array and compute size of index array
      vector<size_t> sizes; size_t num_indices = 0;
      XMLElement* e_vcount = e_polylist->FirstChildElement( "vcount" );
      if ( e_vcount ) {

        size_t size;
        string s = e_vcount->GetText();
        stringstream ss (s);

        for (size_t i = 0; i < num_polygons; ++i) {
          ss >> size;
          sizes.push_back(size);
          num_indices += size * stride;
        }

      } else {
        stat("Error: polygon sizes undefined in geometry: " << polymesh.id);
        exit( -1 );
      }

      // index array
      vector<size_t> indices;
      XMLElement* e_p = e_polylist->FirstChildElement( "p" );
      if ( e_p ) {

        size_t index;
        string s = e_p->GetText();
        stringstream ss (s);

        for (size_t i = 0; i < num_indices; ++i) {
          ss >> index;
          indices.push_back(index);
        }

      } else {
        stat("Error: no index array defined in geometry: " << polymesh.id);
        exit( -1 );
      }

      // create polygons
      polymesh.polygons.resize(num_polygons);

      // vertex array indices
      if (has_vertex_array) {
        size_t k = 0;
        for (size_t i = 0; i < num_polygons; ++i) {
          for (size_t j = 0; j < sizes[i]; ++j) {
            polymesh.polygons[i].vertex_indices.push_back(
              indices[k * stride + vertex_offset]
            );
            k++;
          }
        }
      }

      // normal array indices
      if (has_normal_array) {
        size_t k = 0;
        for (size_t i = 0; i < num_polygons; ++i) {
          for (size_t j = 0; j < sizes[i]; ++j) {
            polymesh.polygons[i].normal_indices.push_back(
              indices[k * stride + normal_offset]
            );
            k++;
          }
        }
      }

      // texcoord array indices
      if (has_normal_array) {
        size_t k = 0;
        for (size_t i = 0; i < num_polygons; ++i) {
          for (size_t j = 0; j < sizes[i]; ++j) {
            polymesh.polygons[i].texcoord_indices.push_back(
              indices[k * stride + texcoord_offset]
            );
            k++;
          }
        }
      }

    }
  }

  // print summary
  stat("  |- " << polymesh);
}

void ColladaParser::parseMaterial ( XMLElement* xml, Material& material ) {

  // name & id
  material.id   = xml->Attribute(  "id"  );
  material.name = xml->Attribute( "name" );
  material.type = MATERIAL;

  // parse material effect
  XMLElement* e_instance_effect = xml->FirstChildElement( "instance_effect" );
  if ( e_instance_effect ) {

    const char* instance_url = e_instance_effect->Attribute("url");
    if ( instance_url ) {

      string effect_id = instance_url + 1;
      XMLElement* e_effect = find_instance( e_effects, effect_id );
      if ( e_effect ) {

        // Note (sky):
        // right now this only supports effect/profile_COMMON/technique/phong
        XMLElement* e_profile = e_effect->FirstChildElement( "profile_COMMON" );
        if ( !e_profile ) {
          stat("Error: no common profile defined for material: " << material.id);
          exit( -1 );
        }

        XMLElement* e_technique = e_profile->FirstChildElement( "technique" );
        if ( !e_technique ) {
          stat("Error: no technique defined for common profile in material: " << material.id);
          exit( -1 );
        }

        XMLElement* e_phong = e_technique->FirstChildElement( "phong" );
        if ( !e_phong ) {
          stat("Error: no phong shading is defined for material: " << material.id);
          exit( -1 );
        }

        // parse phong shading model
        XMLElement* e_emit = e_phong->FirstChildElement( "emission"  );
        XMLElement* e_ambi = e_phong->FirstChildElement( "ambient"   );
        XMLElement* e_diff = e_phong->FirstChildElement( "diffuse"   );
        XMLElement* e_spec = e_phong->FirstChildElement( "specular"  );
        XMLElement* e_shin = e_phong->FirstChildElement( "shininess" );
        XMLElement* e_refi = e_phong->FirstChildElement( "index_of_refraction" );

        XMLElement* e_emit_val = e_emit ? e_emit->FirstChildElement( "color" ) : NULL;
        XMLElement* e_ambi_val = e_ambi ? e_ambi->FirstChildElement( "color" ) : NULL;
        XMLElement* e_diff_val = e_diff ? e_diff->FirstChildElement( "color" ) : NULL;
        XMLElement* e_spec_val = e_spec ? e_spec->FirstChildElement( "color" ) : NULL;
        XMLElement* e_shin_val = e_shin ? e_shin->FirstChildElement( "float" ) : NULL;
        XMLElement* e_refi_val = e_refi ? e_refi->FirstChildElement( "float" ) : NULL;

        string emit_str = e_emit_val ? e_emit_val->GetText() : "0 0 0 0";
        string ambi_str = e_ambi_val ? e_ambi_val->GetText() : "0 0 0 0";
        string diff_str = e_diff_val ? e_diff_val->GetText() : "0 0 0 0";
        string spec_str = e_spec_val ? e_spec_val->GetText() : "0 0 0 0";

        material.emit = rgba_from_string( emit_str );
        material.ambi = rgba_from_string( ambi_str );
        material.diff = rgba_from_string( diff_str );
        material.spec = rgba_from_string( spec_str );

        material.shininess = e_shin_val ? atof(e_shin_val->GetText()) : 0.0f;
        material.refractive_index = e_refi_val ? atof(e_refi_val->GetText()) : 1.0f;

      } else {
        stat("Error: undefined effect instance: " << effect_id);
        exit( -1 );
      }

    } else {
      // handle non-indirection
    }
  }

  // print summary
  stat("  |- " << material);
}


} // namespace CMU462
